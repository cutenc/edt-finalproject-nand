/*
 * octree_tickets.hpp
 *
 *  Created on: 19/set/2012
 *      Author: socket
 */

#ifndef OCTREE_TICKETS_HPP_
#define OCTREE_TICKETS_HPP_

#include "octree_nodes.hpp"

class OctreeTicket {
	
public:
	virtual ~OctreeTicket() { }
	
	/* TODO: fra i tipi di ticket ci sono vari accrocchi per far eseguire un
	 * altro ticket quando il corrente conclude: ipotizzare che performAction
	 * restituisca una lista di altri ticket da portare a termine una volta
	 * terminato quello corrente
	 */
	
	virtual void performAction() const =0;
	
};

template < typename LeafType >
class OctreeLeafTicket : public OctreeTicket {
	
protected:
	typedef typename LeafType::Ptr LeafPtr;
	
	const LeafPtr targetLeaf;
	
public:
	OctreeLeafTicket(const LeafPtr &leaf) : targetLeaf(leaf) {
		assert(leaf != NULL);
	}
	
	virtual ~OctreeLeafTicket() { }
	
	LeafPtr getTarget() const { return this->targetLeaf; }
	
};




class PurgeNodeTicket : public OctreeTicket {

private:
	const OctreeNode::Ptr node;
	
public:
	PurgeNodeTicket(const OctreeNode::Ptr &node) : node(node) { }
	virtual ~PurgeNodeTicket() { }
	
	virtual void performAction() const {
		purgeNode(node);
	}
	
	static void purgeNode(OctreeNode::Ptr node) {
		assert(node != NULL);
		
		// tells father to forget its children
		BranchNode::Ptr bnp = static_cast< BranchNode::Ptr >(node->getFather());
		bnp->deleteChild(node->getChildIdx());
		
		// then free leaf's memory (no longer needed)
		delete node;
		node = NULL;
	}
};




template < typename LeafType >
class PushLevelTicket : public OctreeLeafTicket< LeafType > {
	
private:
	typedef typename OctreeLeafTicket< LeafType >::LeafPtr LeafPtr;
	
	BranchNode::Ptr newBranch;
	
public:
	PushLevelTicket(const LeafPtr leaf, const BranchNode::Ptr &newBranch) :
		OctreeLeafTicket< LeafType >(leaf), newBranch(newBranch) {
	}
	
	virtual ~PushLevelTicket() { }
	
	virtual void performAction() const {
		LeafPtr target = OctreeLeafTicket< LeafType >::getTarget();
		attachBranch(target, newBranch);
	}
	
	/**
	 * 
	 * @param leaf
	 * @param newBranch
	 * @return \c true if given \c leaf should be deleted because floating
	 * point approximation error cause all \c newBranch children to be deleted
	 */
	static void attachBranch(LeafPtr leaf, const BranchNode::Ptr newBranch) {
		assert(newBranch != NULL);
		
		BranchNode::Ptr father = static_cast< BranchNode::Ptr >(leaf->getFather());
		assert(father == newBranch->getFather());
		
		u_char leafIdx = leaf->getChildIdx();
		
		// then delete leaf from father
		father->deleteChild(leafIdx);
		
		// then set newBranch as father's child
		father->setChild(leafIdx, newBranch);
		
		// then free leaf's memory (no longer needed)
		delete leaf;
		leaf = NULL;
	}
	
};




template < typename LeafType >
class UpdateDataTicket : public OctreeLeafTicket< LeafType > {
	
public:
	typedef typename LeafType::DataConst DataConst;
	typedef typename LeafType::DataConstRef DataConstRef;
	
private:
	typedef typename OctreeLeafTicket< LeafType >::LeafPtr LeafPtr;
	
private:
	DataConst newData;
	
public:
	UpdateDataTicket(const LeafPtr leaf, DataConstRef newData) :
		OctreeLeafTicket< LeafType >(leaf), newData(newData) { }
	
	virtual ~UpdateDataTicket() { }
	
	virtual void performAction() const {
		updateData(OctreeLeafTicket< LeafType >::getTarget(), newData);
	}
	
	static void updateData(LeafPtr leaf, DataConstRef newData) {
		assert(leaf != NULL);
		
		leaf->setData(newData);
	}
	
};

#endif /* OCTREE_TICKETS_HPP_ */
