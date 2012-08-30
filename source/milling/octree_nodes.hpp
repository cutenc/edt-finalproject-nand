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

#include <Eigen/Geometry>

#include "common/Point3D.hpp"

enum OctreeNodeType {
	BRANCH_NODE,
	LEAF_NODE
};

class OctreeNode;
typedef OctreeNode * OctreeNodePtr;

class OctreeNode {
	
	const OctreeNodePtr father;
	const u_char childIdx;
	const Eigen::Vector3d traslation;
	
public:
	OctreeNode() : father(), childIdx(255), traslation(0, 0, 0) { }
	
	OctreeNode(OctreeNodePtr father, u_char childIdx, const Eigen::Vector3d &traslation) :
			father(father), childIdx(childIdx), traslation(traslation) {
		
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
	
	Eigen::Vector3d getTraslation() const {
		return this->traslation;
	}
	
};


class BranchNode : public OctreeNode {
	
public:
	static const u_char N_CHILDREN = 8;
	
private:
	OctreeNodePtr children[N_CHILDREN];
	
public:
	BranchNode() : OctreeNode() { initChildren(); }
	BranchNode(OctreeNodePtr father, u_char childIdx, const Eigen::Vector3d &traslation) : 
		OctreeNode(father, childIdx, traslation) { initChildren(); }
	
	virtual ~BranchNode() {
		for (int i = 0; i < N_CHILDREN; i++) {
			if (children[i] != NULL)
				delete children[i];
		}
	}
	
	virtual OctreeNodeType getType() const {
		return BRANCH_NODE;
	}
	
	OctreeNodePtr getChild(int i) const {
		if (i < 0 || i > 7)
			throw std::invalid_argument("given index should be in [0, 7]");
		
		return children[i];
	}
	
	void setChild(int i, OctreeNodePtr child) {
		if (i < 0 || i > 7)
			throw std::invalid_argument("given index should be in [0, 7]");
		
		(this->children)[i] = child;
	}
	
private:
	void initChildren() {
		for (int i = 0; i < N_CHILDREN; i++)
			children[i] = NULL;
	}
};

template <typename DataT>
class LeafNode : public OctreeNode {
	
	typedef LeafNode<DataT> * LeafPtr;
	
	LeafPtr prev, next;
	const u_int DEPTH;
	
	DataT data;
	
public:
	LeafNode() : OctreeNode(), DEPTH(0) { }
	LeafNode(OctreeNodePtr father, u_char childIdx, const Eigen::Vector3d &traslation,
			u_int depth) : OctreeNode(father, childIdx, traslation), DEPTH(depth) {
		
		prev = next = NULL;
	}
	
	virtual ~LeafNode() { }
	
	virtual OctreeNodeType getType() const {
		return LEAF_NODE;
	}
	
	void setData(const DataT &data) {
		this->data = data;
	}
	
	DataT getData() const {
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
		this->next = next;
	}
	
	u_int getDepth() const {
		return this->DEPTH;
	}
	
	SimpleBox getSimpleBox(Eigen::Vector3d totalExtent) const {
		u_int rate = (0x01 << DEPTH);
		return SimpleBox(totalExtent / rate);
	}
};


#endif /* OCTREE_NODES_HPP_ */
