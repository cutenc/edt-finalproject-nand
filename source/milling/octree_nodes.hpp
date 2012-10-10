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

class OctreeNode {
	
public:
	typedef OctreeNode * Ptr;
	typedef const OctreeNode * ConstPtr;
	
	enum OctreeNodeType {
		BRANCH_NODE,
		LEAF_NODE
	};
	
private:
	struct NodeIDs {
		static u_long getNodeID() {
			static AtomicNumber< u_long > IDs;
			return IDs.getAndInc();
		}
	};
	
	const OctreeNode::Ptr father;
	const u_char childIdx;
	const ShiftedBox::ConstPtr sbox;
	const u_long NODE_ID;
	
public:
	OctreeNode(const ShiftedBox::ConstPtr &box) : father(), childIdx(255), sbox(box),
			NODE_ID(NodeIDs::getNodeID())
	{ }
	
	OctreeNode(const OctreeNode::Ptr &father, u_char childIdx, const ShiftedBox::ConstPtr &sbox) :
			father(father), childIdx(childIdx), sbox(sbox), NODE_ID(NodeIDs::getNodeID())
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
	u_char getChildIdx() const {
		assert(!isRoot());
			// throw std::runtime_error("cannot ask root's childIdx");
		
		return this->childIdx;
	}
	
	inline
	const ShiftedBox::ConstPtr & getBox() const {
		return this->sbox;
	}
	
	inline
	virtual u_int getDepth() const {
		if (isRoot())
			return 0;
		else
			return 1 + getFather()->getDepth();
	}
	
	inline
	u_long getID() const {
		return this->NODE_ID;
	}
	
	virtual std::ostream & toOutStream(std::ostream &os) const =0;
	
	friend std::ostream & operator<<(std::ostream & os, const OctreeNode& node) {
		return node.toOutStream(os);
	}
};


class BranchNode : public OctreeNode {
	
public:
	typedef BranchNode * Ptr;
	typedef const BranchNode * ConstPtr;
	
public:
	static const u_char N_CHILDREN = 8;
	
private:
	boost::array< OctreeNode::Ptr, N_CHILDREN > children;
	u_char removedChildren;
	
public:
	BranchNode(const ShiftedBox::ConstPtr &box) : OctreeNode(box) { initChildren(); }
	BranchNode(OctreeNode::Ptr father, u_char childIdx, const ShiftedBox::ConstPtr &sbox) : 
		OctreeNode(father, childIdx, sbox) { initChildren(); }
	
	virtual ~BranchNode() {
		for (u_char i = 0; i < N_CHILDREN; i++) {
			if (children[i] != NULL)
				delete children[i];
		}
	}
	
	inline
	virtual OctreeNodeType getType() const {
		return BRANCH_NODE;
	}
	
	inline
	bool hasChild(u_char i) const {
		return children[i] != NULL;
	}
	
	inline
	OctreeNode::Ptr getChild(u_char i) const {
		assert(hasChild(i));
		
		return children[i];
	}
	
	/**
	 * 
	 * @return \c true if this branch has no more children
	 */
	inline
	bool isEmpty() const {
		return removedChildren == N_CHILDREN;
	}
	
	/**
	 * Returns true if this branch should be deleted (because all its
	 * children has been deleted)
	 * @param i
	 * @return
	 */	
	inline
	bool deleteChild(u_char i) {
		assert(hasChild(i));
		
		removedChildren++;
		
		children[i] = NULL;
		return isEmpty();
	}
	
	inline
	void setChild(u_char i, const OctreeNode::Ptr &child) {
		assert(!hasChild(i));
		
		removedChildren--;
		
		(this->children)[i] = child;
	}
	
	/**
	 * Returns the first (left-most) child node with given type or \c NULL if
	 * no such child can be found
	 * @param type
	 * @return
	 */
	inline
	OctreeNode::Ptr getFirst(OctreeNodeType type) const {
		for (u_char i = 0; i < N_CHILDREN; ++i) {
			if (hasChild(i)) {
				OctreeNode::Ptr child = getChild(i);
				if (child->getType() == type)
					return child;
				
				if (child->getType() == this->getType())
					return (dynamic_cast< BranchNode::Ptr >(child))->getFirst(type);
			}
		}
		
		return NULL;
	}
	
	/**
	 * Returns the first (left-most) child node with given type or \c NULL if
	 * no such child can be found
	 * @param type
	 * @return
	 */
	inline
	OctreeNode::Ptr getLast(OctreeNodeType type) const {
		for (int i = N_CHILDREN - 1; i >= 0; --i) {
			if (hasChild(i)) {
				OctreeNode::Ptr child = getChild(i);
				if (child->getType() == type)
					return child;
				
				if (child->getType() == this->getType())
					return (dynamic_cast< BranchNode::Ptr >(child))->getLast(type);
			}
		}
		
		return NULL;
	}
	
	virtual std::ostream & toOutStream(std::ostream &os) const {
		u_int depth = this->getDepth();
		
		std::cout << "depth calculated" << std::endl;
		
		std::string tabs = StringUtils::repeat("\t", depth);
		
		std::cout << "tabs ok " << std::endl;
		
		os << "Branch-" << this->getID() << "@" << depth;
		
		std::cout << "branch signature" << std::endl;
		
		for (u_char i = 0; i < this->N_CHILDREN; i++) {
			os << std::endl << tabs << "|->" << (int)i << "-";
			
			std::cout << "first line ok" << std::endl;
			
			if (hasChild(i)) {
				os << *(getChild(i));
			} else {
				os << "DELETED";
			}
			
			std::cout << "if ok" << std::endl;
		}
		return os;
	}
	
private:
	void initChildren() {
		for (u_char i = 0; i < N_CHILDREN; i++)
			children[i] = NULL;
		
		removedChildren = N_CHILDREN;
	}
};

template < typename DataT >
struct DataTraits {
	typedef DataT type;
	typedef const DataT const_type;
	typedef DataT & reference;
	typedef const DataT & const_reference;
	
	static const_reference DEFAULT_DATA() {
		static const_type DATA;
		return DATA;
	}
};

template <typename DataT, typename data_traits = DataTraits< DataT> >
class LeafNode : public OctreeNode {
	
public:
	typedef LeafNode< DataT, data_traits > * Ptr;
	typedef const LeafNode< DataT, data_traits > * ConstPtr;
	
	typedef typename data_traits::const_type DataConst;
	typedef typename data_traits::reference DataRef;
	typedef typename data_traits::const_reference DataConstRef;
	
private:
	typedef LeafNode< DataT, data_traits >::Ptr LeafPtr;
	
private:
	
	const u_int DEPTH;
	const u_int VERSION;
	
	DataConst data;
	
public:
	LeafNode(const ShiftedBox::ConstPtr &box) :
			OctreeNode(box),
			DEPTH(0), VERSION(-1) {
		
		initVariables();
	}
	
	LeafNode(const OctreeNode::Ptr &father, u_char childIdx, 
			const ShiftedBox::ConstPtr &sbox, u_int depth, u_int version) :
				OctreeNode(father, childIdx, sbox),
				DEPTH(depth), VERSION(version) {
		
		initVariables();
	}
	
	virtual ~LeafNode() { }
	
	inline
	virtual OctreeNodeType getType() const {
		return LEAF_NODE;
	}
	
	inline
	DataConstRef getData() const {
		return this->data;
	}
	
	inline
	u_int getVersion() const {
		return this->VERSION;
	}
	
	inline
	void setData(DataConstRef data) {
		this->data = data;
	}
	
	inline
	virtual u_int getDepth() const {
		return this->DEPTH;
	}
	
	virtual std::ostream & toOutStream(std::ostream &os) const {
		os << "Leaf-" << this->getID() << "@" << getDepth() << ":";
		os << this->data;
		
		return os;
	}

private:
	
	void initVariables() {
		data = data_traits::DEFAULT_DATA();
	}
	
};


#endif /* OCTREE_NODES_HPP_ */
