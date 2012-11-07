/*
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
	OctreeNode(const ShiftedBox::ConstPtr &box, const VersionInfo &vinfo) :
		father(), childIdx(255), sbox(box), NODE_ID(NodeIDs::getNodeID()),
		DEPTH(0), firstChangeVersion(vinfo.currVersion)
	{ }
	
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
	
	virtual ~OctreeNode() { }
	
	virtual OctreeNodeType getType() const =0;
	
	inline
	bool isRoot() const { return (this->father == NULL); }
	
	inline
	OctreeNode::Ptr getFather() const {
		assert(!isRoot());
			// throw std::runtime_error("cannot ask root's father");
		
		return this->father;
	}
	
	inline
	unsigned char getChildIdx() const {
		assert(!isRoot());
			// throw std::runtime_error("cannot ask root's childIdx");
		
		return this->childIdx;
	}
	
	inline
	const ShiftedBox::ConstPtr & getBox() const {
		return this->sbox;
	}
	
	inline
	unsigned int getDepth() const {
		return this->DEPTH;
	}
	
	inline
	unsigned long getID() const {
		return this->NODE_ID;
	}
	
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
	
	friend std::ostream & operator<<(std::ostream & os, const OctreeNode& node) {
		return node.toOutStream(os);
	}
	
	OctreeNode::Ptr getAdjacent(Adjacencies::Direction dir) {
		assert(!isRoot());
		
		std::vector<unsigned char> path;
		return getFather()->getAdjacentUp(getChildIdx(), dir, path);
	}
	
	virtual OctreeNode::Ptr getAdjacentUp(unsigned char childIdx,
			const Adjacencies::Direction &dir,
			std::vector< unsigned char > &path) =0;
	
	virtual OctreeNode::Ptr getAdjacentDown(std::vector< unsigned char > &path) =0;
};


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
	BranchNode(const ShiftedBox::ConstPtr &box, const VersionInfo &vinfo) :
		OctreeNode(box, vinfo)
	{ initChildren(); }
	
	BranchNode(OctreeNode::Ptr father, unsigned char childIdx, const ShiftedBox::ConstPtr &sbox, const VersionInfo &vinfo) :
		OctreeNode(father, childIdx, sbox, vinfo) { initChildren(); }
	
	virtual ~BranchNode() {
		for (int i = 0; i < N_CHILDREN; i++) {
			if (hasChild(i))
				delete children[i];
		}
	}
	
	inline
	virtual OctreeNodeType getType() const {
		return BRANCH_NODE;
	}
	
	inline
	bool hasChild(int i) const {
		assert(i >= 0 && i < N_CHILDREN);
		return childrenMask & (0x01 << i);
	}
	
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


class LeafNode : public OctreeNode {
	
public:
	typedef LeafNode * Ptr;
	typedef const LeafNode * ConstPtr;
	
private:
	unsigned char cuttedVertex;
	VoxelInfo::Ptr voxelInfo;
	
public:
	LeafNode(const ShiftedBox::ConstPtr &box, const VersionInfo &vinfo) :
			OctreeNode(box, vinfo)
	{
	}
	
	LeafNode(const OctreeNode::Ptr &father, unsigned char childIdx,
			const ShiftedBox::ConstPtr &sbox, const VersionInfo &vinfo) :
				OctreeNode(father, childIdx, sbox, vinfo),
				voxelInfo(boost::make_shared< VoxelInfo >(VoxelInfo::DEFAULT_INSIDENESS()))
	{
	}
	
	virtual ~LeafNode() { }
	
	inline
	virtual OctreeNodeType getType() const {
		return LEAF_NODE;
	}
	
	VoxelInfo::Ptr getData() {
		return this->voxelInfo;
	}
	
	virtual std::ostream & toOutStream(std::ostream &os) const {
		return os << *voxelInfo;
	}
	
	virtual OctreeNode::Ptr getAdjacentUp(unsigned char, const Adjacencies::Direction &,
			std::vector< unsigned char > &) {
		throw std::runtime_error("Adjacent up request to a leaf");
	}
	
	virtual OctreeNode::Ptr getAdjacentDown(std::vector< unsigned char > &) {
		return this;
	}
};


#endif /* OCTREE_NODES_HPP_ */
