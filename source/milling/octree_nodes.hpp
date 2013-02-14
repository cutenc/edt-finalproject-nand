/**
 * octree_nodes.hpp
 *
 *  Created on: 22/ago/2012
 *      Author: socket
 */

#ifndef OCTREE_NODES_HPP_
#define OCTREE_NODES_HPP_

#include <vector>
#include <list>
#include <stdexcept>

#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include <Eigen/Geometry>

#include "common/Point3D.hpp"
#include "common/Utilities.hpp"
#include "ShiftedBox.hpp"
#include "Adjacencies.hpp"
#include "VoxelInfo.hpp"

/**
 * @class OctreeNode
 *
 * defines a node of the octree
 */
class OctreeNode {
	
public:
	typedef OctreeNode * Ptr;
	typedef const OctreeNode * ConstPtr;
	
	enum OctreeNodeType {
		BRANCH_NODE = 0,
		LEAF_NODE
	};
	
	struct VersionInfo {
		VersionInfo() : minChangeVersion(0), currVersion(0) { }
		VersionInfo(unsigned int minVersion, unsigned int currVersion) :
			minChangeVersion(minVersion), currVersion(currVersion) { }
		
		virtual ~VersionInfo() { }
		
		const unsigned int minChangeVersion;
		const unsigned int currVersion;
	};
	
private:
	struct NodeIDs {
		static unsigned long getNodeID() {
			static AtomicNumber< unsigned long > IDs;
			return IDs.getAndInc();
		}
	};
	
	const OctreeNode::Ptr father;
	const unsigned char childIdx;
	const ShiftedBox::ConstPtr sbox;
	const unsigned long NODE_ID;
	const unsigned int DEPTH;
	
	unsigned int firstChangeVersion;
	
public:
	/**
	 * constructor
	 * @param box
	 * @param vinfo
	 */
	OctreeNode(const ShiftedBox::ConstPtr &box, const VersionInfo &vinfo) :
		father(), childIdx(255), sbox(box), NODE_ID(NodeIDs::getNodeID()),
		DEPTH(0), firstChangeVersion(vinfo.currVersion)
	{ }
	
	/**
	 * constructor
	 * @param father
	 * @param childIdx
	 * @param sbox
	 * @param vinfo
	 */
	OctreeNode(const OctreeNode::Ptr &father, unsigned char childIdx,
			const ShiftedBox::ConstPtr &sbox, const VersionInfo &vinfo) :
			father(father), childIdx(childIdx), sbox(sbox),
			NODE_ID(NodeIDs::getNodeID()), DEPTH(father->getDepth() + 1),
			firstChangeVersion(vinfo.currVersion)
	{
		
		if (father == NULL)
			throw std::invalid_argument("Given father cannot be null");
		if (childIdx >= 8)
			throw std::invalid_argument("Given childIdx must be in [0, 7]");
	}
	
	/**
	 * destructor
	 */
	virtual ~OctreeNode() { }
	
	/**
	 *
	 * @return the node type
	 */
	virtual OctreeNodeType getType() const =0;
	
	/**
	 *
	 * @return True if the node is the root of the octree
	 */
	inline
	bool isRoot() const { return (this->father == NULL); }
	
	/**
	 *
	 * @return pointer to the ancestor of the node
	 */
	inline
	OctreeNode::Ptr getFather() const {
		assert(!isRoot());
			// throw std::runtime_error("cannot ask root's father");
		
		return this->father;
	}
	
	/**
	 *
	 * @return childIdx
	 */
	inline
	unsigned char getChildIdx() const {
		assert(!isRoot());
			// throw std::runtime_error("cannot ask root's childIdx");
		
		return this->childIdx;
	}
	
	/**
	 *
	 * @return the ShiftedBox of the node
	 */
	inline
	const ShiftedBox::ConstPtr & getBox() const {
		return this->sbox;
	}
	
	/**
	 *
	 * @return depth of the node
	 */
	inline
	unsigned int getDepth() const {
		return this->DEPTH;
	}
	
	/**
	 *
	 * @return ID of the node
	 */
	inline
	unsigned long getID() const {
		return this->NODE_ID;
	}
	
	/**
	 * set first change version
	 * @param vinfo
	 */
	void setFirstChangeVersion(const VersionInfo &vinfo) {
		if (isChanged(vinfo)) {
			// we already set our 'first change' version
			return;
		}
		
		firstChangeVersion = vinfo.currVersion;
		
		if (isRoot()) {
			return;
		} else {
			getFather()->setFirstChangeVersion(vinfo);
		}
	}
	
	/**
	 * 
	 * @param vinfo
	 * @return \c true if firstChangeVersion occurs in the
	 * ]vinfo.minChangeVersion; +inf] interval
	 */
	bool isChanged(const VersionInfo &vinfo) const {
		return firstChangeVersion > vinfo.minChangeVersion;
	}
	
	virtual std::ostream & toOutStream(std::ostream &os) const =0;
	
	/**
	 * overrides <<
	 * @param os
	 * @param node
	 * @return
	 */
	friend std::ostream & operator<<(std::ostream & os, const OctreeNode& node) {
		return node.toOutStream(os);
	}
	
	/**
	 *
	 * @param dir
	 * @return pointer to the adjacent node in the given direction
	 */
	OctreeNode::Ptr getAdjacent(Adjacencies::Direction dir) {
		assert(!isRoot());
		
		std::vector<unsigned char> path;
		return getFather()->getAdjacentUp(getChildIdx(), dir, path);
	}
	
	/**
	 *
	 * @param childIdx
	 * @param dir
	 * @param path
	 * @return pointer to the adjacent node in upper level
	 */
	virtual OctreeNode::Ptr getAdjacentUp(unsigned char childIdx,
			const Adjacencies::Direction &dir,
			std::vector< unsigned char > &path) =0;
	
	/**
	 *
	 * @param path
	 * @return pointer to the adjacent node in the lower level
	 */
	virtual OctreeNode::Ptr getAdjacentDown(std::vector< unsigned char > &path) =0;
};

/**
 * @class BranchNode
 *
 * internal node of the octree
 */
class BranchNode : public OctreeNode {
	
public:
	typedef BranchNode * Ptr;
	typedef const BranchNode * ConstPtr;
	
public:
	static const int N_CHILDREN = 8;
	
private:
	boost::array< OctreeNode::Ptr, N_CHILDREN > children;
	
	/*
	 * bit = 0 => children absent
	 * bit = 1 => children present
	 */
	unsigned char childrenMask;
	
public:
	/**
	 * constructor
	 * @param box
	 * @param vinfo
	 */
	BranchNode(const ShiftedBox::ConstPtr &box, const VersionInfo &vinfo) :
		OctreeNode(box, vinfo)
	{ initChildren(); }
	
	/**
	 * constructor
	 * @param father
	 * @param childIdx
	 * @param sbox
	 * @param vinfo
	 */
	BranchNode(OctreeNode::Ptr father, unsigned char childIdx, const ShiftedBox::ConstPtr &sbox, const VersionInfo &vinfo) :
		OctreeNode(father, childIdx, sbox, vinfo) { initChildren(); }
	
	/**
	 * destructor
	 */
	virtual ~BranchNode() {
		for (int i = 0; i < N_CHILDREN; i++) {
			if (hasChild(i))
				delete children[i];
		}
	}
	
	/**
	 *
	 * @return the type of the node (internal)
	 */
	inline
	virtual OctreeNodeType getType() const {
		return BRANCH_NODE;
	}
	
	/**
	 *
	 * @param i
	 * @return True if the node has children
	 */
	inline
	bool hasChild(int i) const {
		assert(i >= 0 && i < N_CHILDREN);
		return childrenMask & (0x01 << i);
	}
	
	/**
	 *
	 * @param i
	 * @return selected children
	 */
	inline
	OctreeNode::Ptr getChild(int i) const {
		assert(hasChild(i));
		return children[i];
	}
	
	/**
	 * 
	 * @return \c true if this branch has no more children
	 */
	inline
	bool isEmpty() const {
		return !childrenMask;
	}
	
	/**
	 * Returns true if this branch should be deleted (because all its
	 * children has been deleted)
	 * @param i
	 * @return
	 */	
	inline
	void deleteChild(int i) {
		assert(hasChild(i));
		
		childrenMask &= ~(0x01 << i);
		children[i] = NULL;
	}
	
	/**
	 * set a child to the assigned branch
	 * @param i
	 * @param child
	 */
	inline
	void setChild(int i, const OctreeNode::Ptr &child) {
		assert(!hasChild(i));
		
		childrenMask |= (0x01 << i);
		(this->children)[i] = child;
	}
	
	virtual std::ostream & toOutStream(std::ostream &os) const {
		unsigned int depth = this->getDepth();
		
		std::string tabs = StringUtils::repeat("\t", depth);
		os << "Branch-" << this->getID() << "@" << depth;
		for (int i = 0; i < this->N_CHILDREN; i++) {
			os << std::endl << tabs << "|->" << (int)i << "-";
			if (hasChild(i)) {
				os << *(getChild(i));
			} else {
				os << "DELETED";
			}
			
			std::cout << "if ok" << std::endl;
		}
		return os;
	}
	
	/**
	 *
	 * @param childIdx
	 * @param dir
	 * @param path
	 * @return pointer to the adjacent node in upper level
	 */
	virtual OctreeNode::Ptr getAdjacentUp(unsigned char childIdx, const Adjacencies::Direction &dir,
			std::vector< unsigned char > &path) {
		
		Adjacencies::Adjacency adj = Adjacencies::getAdjacent(
				childIdx,
				dir
		);
		
		switch (adj.second) {
			case Adjacencies::LOCAL: {
				if (hasChild(adj.second)) {
					return getChild(adj.second)->getAdjacentDown(path);
				} else {
					/* there's a hole in the octree structure: the nearest
					 * voxel in asked direction is this one.
					 */
					return this;
				}
			}
			
			case Adjacencies::EXTERN: {
				if(isRoot()) {
					// asking for a voxel outside the octree: there's none
					return NULL;
				}
				
				path.push_back(adj.second);
				return getFather()->getAdjacentUp(getChildIdx(), dir, path);
			}
			
			default:
				throw std::runtime_error("Unknown adj type");
		}
	}
	
	/**
	 *
	 * @param path
	 * @return pointer to the adjacent node in lower level
	 */
	virtual OctreeNode::Ptr getAdjacentDown(std::vector< unsigned char > &path) {
		if (path.empty()) {
			return this;
		}
		
		unsigned char pChild = path.back(); path.pop_back();
		if (hasChild(pChild)) {
			return getChild(pChild)->getAdjacentDown(path);
		} else {
			return this;
		}
		
	}
	
private:
	void initChildren() {
		childrenMask = 0x00;
	}
};

/**
 * @class LeafNode
 *
 * leaf node
 */
class LeafNode : public OctreeNode {
	
public:
	typedef LeafNode * Ptr;
	typedef const LeafNode * ConstPtr;
	
private:
	unsigned char cuttedVertex;
	VoxelInfo::Ptr voxelInfo;
	
public:
	/**
	 * constructor
	 *
	 * @param box
	 * @param vinfo
	 */
	LeafNode(const ShiftedBox::ConstPtr &box, const VersionInfo &vinfo) :
			OctreeNode(box, vinfo)
	{
	}
	
	/**
	 * constructor
	 *
	 * @param father
	 * @param childIdx
	 * @param sbox
	 * @param vinfo
	 */
	LeafNode(const OctreeNode::Ptr &father, unsigned char childIdx,
			const ShiftedBox::ConstPtr &sbox, const VersionInfo &vinfo) :
				OctreeNode(father, childIdx, sbox, vinfo),
				voxelInfo(boost::make_shared< VoxelInfo >(VoxelInfo::DEFAULT_INSIDENESS()))
	{
	}
	
	/**
	 * destructor
	 */
	virtual ~LeafNode() { }
	
	/**
	 *
	 * @return type of the node (internal)
	 */
	inline
	virtual OctreeNodeType getType() const {
		return LEAF_NODE;
	}
	
	/**
	 *
	 * @return the VoxelInfos
	 */
	VoxelInfo::Ptr getData() {
		return this->voxelInfo;
	}
	
	virtual std::ostream & toOutStream(std::ostream &os) const {
		return os << *voxelInfo;
	}
	
	/**
	 *
	 * @param childIdx
	 * @param dir
	 * @param path
	 * @return pointer to the adjacent node in upper level
	 */
	virtual OctreeNode::Ptr getAdjacentUp(unsigned char, const Adjacencies::Direction &,
			std::vector< unsigned char > &) {
		throw std::runtime_error("Adjacent up request to a leaf");
	}
	
	/**
	 *
	 * @param path
	 * @return pointer to the adjacent node in lower level
	 */
	virtual OctreeNode::Ptr getAdjacentDown(std::vector< unsigned char > &) {
		return this;
	}
};


#endif /* OCTREE_NODES_HPP_ */
