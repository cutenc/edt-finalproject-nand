/*
 * octree_tickets.hpp
 *
 *  Created on: 19/set/2012
 *      Author: socket
 */

#ifndef OCTREE_TICKETS_HPP_
#define OCTREE_TICKETS_HPP_

#include "octree_nodes.hpp"

template < typename T >
class OctreeTicket {
	
private:
	typedef typename LeafNode< T >::LeafPtr LeafPtr;
	
	const LeafPtr targetLeaf;
	
public:
	OctreeTicket(const LeafPtr &leaf) : targetLeaf(leaf) {
		checkNull(targetLeaf);
	}
	
	virtual ~OctreeTicket();
	
	LeafPtr getTarget() const { return this->targetLeaf; }
	
	virtual void performAction() const =0;
	
protected:
	static void checkNull(const void * p) throw(std::invalid_argument) {
		if (p == NULL)
			throw std::invalid_argument("NULL pointer given");
	}
	
};

template < typename T >
class PushLevelTicket : public OctreeTicket< T > {
	
	BranchNode::ConstPtr newBranch;
	
public:
	PushLevelTicket(const LeafPtr &leaf, const BranchNode::ConstPtr &newBranch) :
			OctreeTicket(leaf), newBranch(newBranch) {
		
		OctreeTicket::checkNull(newBranch);
	}
	
	virtual ~PushLevelTicket() { }
	
	virtual void performAction() const {
		attachBranch(getTarget(), newBranch);
	}
	
	static void attachBranch(LeafPtr &leaf, const BranchNode::Ptr &newBranch) {
		checkNull(leaf, newBranch);
		
		BranchNode::Ptr father = dynamic_cast< BranchNode::Ptr >(leaf->getFather());
		assert(father == newBranch->getFather());
		
		LeafPtr firstLeaf = newBranch->getFirst(OctreeNode::LEAF_NODE),
				lastLeaf = newBranch->getLast(OctreeNode::LEAF_NODE);
		u_char leafIdx = leaf->getChildIdx();
		
		// first of all, update leaves list's pointers
		leaf->getPrevious()->setNext(firstLeaf);
		firstLeaf->setPrevious(leaf->getPrevious());
		
		if (leaf->getNext() != NULL) {
			leaf->getNext()->setPrevious(lastLeaf);
			lastLeaf->setNext(leaf->getNext());
		}
		
		// then delete leaf from father
		father->deleteChild(leafIdx);
		
		// then set newBranch as father's child
		father->setChild(leafIdx, newBranch);
		
		// then free leaf's memory (no longer needed)
		delete leaf; leaf = NULL;
	}
	
};

template < typename T >
class PurgeLeafTicket : public OctreeTicket< T > {
	
public:
	PurgeLeafTicket(const LeafPtr &leaf) : OctreeTicket(leaf) { }
	
	virtual ~PurgeLeafTicket() { }
	
	virtual void performAction() const {
		purgeLeaf(getTarget());
	}
	
	static void purgeLeaf(LeafPtr &leaf) {
		
		// adjust previous & next pointers
		if (leaf->getPrevious() != NULL) {
			leaf->getPrevious()->setNext(leaf->getNext());
		}
		if (leaf->getNext() != NULL) {
			leaf->getNext()->setPrevious(leaf->getPrevious());
		}
		
		// tells father to forget her
		BranchNode::Ptr bnp = dynamic_cast< BranchNode::Ptr >(leaf->getFather());
		bnp->deleteChild(leaf->getChildIdx());
		
		// then free leaf's memory (no longer needed)
		delete leaf; leaf = NULL;
		
	}
	
};

#endif /* OCTREE_TICKETS_HPP_ */
