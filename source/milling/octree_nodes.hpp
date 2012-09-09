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

enum OctreeNodeType {
	BRANCH_NODE,
	LEAF_NODE
};

class OctreeNode;
typedef OctreeNode * OctreeNodePtr;
typedef const OctreeNode * OctreeNodeConstPtr;

class OctreeNode {
	
	const OctreeNodePtr father;
	const u_char childIdx;
	const ShiftedBox::ConstPtr sbox;
	
public:
	OctreeNode(SimpleBox::ConstPtr box) : father(), childIdx(255), sbox(box) { }
	
	OctreeNode(OctreeNodePtr father, u_char childIdx, ShiftedBox::ConstPtr sbox) :
			father(father), childIdx(childIdx), sbox(sbox) {
		
		if (father == NULL)
			throw std::invalid_argument("Given father cannot be null");
		if (childIdx >= 8)
			throw std::invalid_argument("Given childIdx must be in [0, 7]");
	}
	
	virtual ~OctreeNode() { }
	
	virtual OctreeNodeType getType() const =0;
	
	bool isRoot() const { return (this->father == NULL); }
	
	OctreeNodePtr getFather() const {
		if (isRoot())
			throw std::runtime_error("cannot ask root's father");
		
		return this->father;
	}
	
	u_char getChildIdx() const {
		if (isRoot())
			throw std::runtime_error("cannot ask root's childIdx");
		
		return this->childIdx;
	}
	
	ShiftedBox::ConstPtr getBox() const {
		return this->sbox;
	}
	
	virtual u_int getDepth() const {
		if (isRoot())
			return 0;
		else
			return 1 + getFather()->getDepth();
	}
	
	virtual std::ostream & toOutStream(std::ostream &os) const =0;
	
	friend std::ostream & operator<<(std::ostream & os, const OctreeNode& node) {
		return node.toOutStream(os);
	}
};


class BranchNode : public OctreeNode {
	
public:
	static const u_char N_CHILDREN = 8;
	
private:
	boost::array< OctreeNodePtr, N_CHILDREN > children;
	
public:
	BranchNode(SimpleBox::ConstPtr box) : OctreeNode(box) { initChildren(); }
	BranchNode(OctreeNodePtr father, u_char childIdx, ShiftedBox::ConstPtr sbox) : 
		OctreeNode(father, childIdx, sbox) { initChildren(); }
	
	virtual ~BranchNode() {
		for (u_char i = 0; i < N_CHILDREN; i++) {
			if (children[i] != NULL)
				delete children[i];
		}
	}
	
	virtual OctreeNodeType getType() const {
		return BRANCH_NODE;
	}
	
	bool hasChild(u_char i) const {
		return children[i] != NULL;
	}
	
	OctreeNodePtr getChild(u_char i) const {
		assert(hasChild(i));
		
		return children[i];
	}
	
	void deleteChild(u_char i) {
		assert(hasChild(i));
		
		// TODO implement swapping & dirtying
		delete children[i];
		children[i] = NULL;
	}
	
	void setChild(u_char i, OctreeNodePtr child) {
		
		// TODO implement swapping & dirtying
		
		(this->children)[i] = child;
	}
	
	virtual std::ostream & toOutStream(std::ostream &os) const {
		u_int depth = this->getDepth();
		std::string tabs = StringUtils::repeat("\t", depth);
		
		os << "Branch@" << depth;
		for (u_char i = 0; i < this->N_CHILDREN; i++) {
			os << std::endl << tabs << "|->" << (int)i << "-";
			if (this->children[i] != NULL) {
				os << *(this->children[i]);
			} else {
				os << "DELETED";
			}
		}
		return os;
	}
	
private:
	void initChildren() {
		for (u_char i = 0; i < N_CHILDREN; i++)
			children[i] = NULL;
	}
};

template <typename DataT>
class LeafNode : public OctreeNode {
	
public:
	typedef boost::shared_ptr< DataT > DataPtr;
	typedef boost::shared_ptr< const DataT > DataConstPtr;
	typedef LeafNode< DataT > * LeafPtr;
	typedef const LeafNode< DataT > * LeafConstPtr;
	
private:
	
	const u_int DEPTH;
	
	LeafPtr prev, next;
	DataPtr data;
	
public:
	LeafNode(SimpleBox::ConstPtr box) : OctreeNode(box), DEPTH(0) { initVariables(); }
	
	LeafNode(OctreeNodePtr father, u_char childIdx, ShiftedBox::ConstPtr sbox,
			u_int depth) : OctreeNode(father, childIdx, sbox), DEPTH(depth) {
		
		initVariables();
	}
	
	virtual ~LeafNode() { }
	
	virtual OctreeNodeType getType() const {
		return LEAF_NODE;
	}
	
	DataConstPtr getData() const {
		return this->data;
	}
	
	void setData(const DataT &data) {
		// TODO implementare swapping & dirtying
		this->data = boost::make_shared< DataT >(data);
	}
	
	DataPtr getDirtyData() {
		// TODO implementare swapping & dirtying
		return this->data;
	}
	
	LeafPtr getPrevious() const {
		return this->prev;
	}
	
	void setPrevious(LeafPtr prev) {
		this->prev = prev;
	}
	
	LeafPtr getNext() const {
		return this->next;
	}
	
	void setNext(LeafPtr next) {
		// NULL next ptr must be allowed (last leaf)
		this->next = next;
	}
	
	virtual u_int getDepth() const {
		return this->DEPTH;
	}
	
	virtual std::ostream & toOutStream(std::ostream &os) const {
		os << "Leaf@" << getDepth() << ":";
		if (this->data.get() == NULL) {
			os << "NO_DATA";
		} else {
			os << *(this->data);
		}
		
		return os;
	}

private:
	
	void initVariables() {
		prev = next = NULL;
		data = boost::make_shared< DataT >();
	}
	
};


#endif /* OCTREE_NODES_HPP_ */
