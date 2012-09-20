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
	
	virtual void performAction() const =0;
	
protected:
	static void checkNull(const void * p) throw(std::invalid_argument) {
		if (p == NULL)
			throw std::invalid_argument("NULL pointer given");
	}
	
};

template < typename T >
class OctreeLeafTicket : public OctreeTicket {
	
protected:
	typedef typename LeafNode< T >::LeafPtr LeafPtr;
	
	const LeafPtr targetLeaf;
	
public:
	OctreeLeafTicket(const LeafPtr &leaf) : targetLeaf(leaf) {
		checkNull(targetLeaf);
	}
	
	virtual ~OctreeLeafTicket() { }
	
	LeafPtr getTarget() const { return this->targetLeaf; }
	
};

template < typename T >
class PushLevelTicket : public OctreeLeafTicket< T > {
	
private:
	typedef typename OctreeLeafTicket< T >::LeafPtr LeafPtr;
	
	BranchNode::Ptr newBranch;
	
public:
	PushLevelTicket(const LeafPtr leaf, const BranchNode::Ptr &newBranch) :
		OctreeLeafTicket< T >(leaf), newBranch(newBranch) {
		
		OctreeTicket::checkNull(newBranch);
	}
	
	virtual ~PushLevelTicket() { }
	
	virtual void performAction() const {
		attachBranch(OctreeLeafTicket< T >::getTarget(), newBranch);
	}
	
	static void attachBranch(LeafPtr leaf, const BranchNode::Ptr newBranch) {
		OctreeTicket::checkNull(newBranch);
		
		BranchNode::Ptr father = dynamic_cast< BranchNode::Ptr >(leaf->getFather());
		assert(father == newBranch->getFather());
		
		LeafPtr firstLeaf = dynamic_cast< LeafPtr >(newBranch->getFirst(OctreeNode::LEAF_NODE)),
				lastLeaf = dynamic_cast< LeafPtr >(newBranch->getLast(OctreeNode::LEAF_NODE));
		
		assert(firstLeaf != NULL && lastLeaf != NULL);
		
		u_char leafIdx = leaf->getChildIdx();
		
		// first of all, update leaves list's pointers
		if (leaf->getPrevious() != NULL) {
			leaf->getPrevious()->setNext(firstLeaf);
			firstLeaf->setPrevious(leaf->getPrevious());
		} else {
			/* this can occur if we push a newly created first-leaf (it has
			 * not been attached to the rest of the tree yet) otherwise it
			 * MUST not occur because due to the presence of a starting fake
			 * leaf there should be always a "previous"
			 */
			assert(leaf->getVersion() == firstLeaf->getVersion());
		}
		
		if (leaf->getNext() != NULL) {
			leaf->getNext()->setPrevious(lastLeaf);
			lastLeaf->setNext(leaf->getNext());
		}
		
		// then delete leaf from father
		father->deleteChild(leafIdx);
		
		// then set newBranch as father's child
		father->setChild(leafIdx, newBranch);
		
		// then free leaf's memory (no longer needed)
		delete leaf;
	}
	
};

class PurgeNodeTicket : public OctreeTicket {

public:
	struct DeletionInfo {
		DeletionInfo(bool b, const OctreeNode::Ptr &p) :
			shouldDelete(b), reference(p)
		{ }
		
		bool shouldDelete;
		OctreeNode::Ptr reference;
	};
	
private:
	const OctreeNode::Ptr node;
	
public:
	PurgeNodeTicket(const OctreeNode::Ptr &node) : node(node) { }
	virtual ~PurgeNodeTicket() { }
	
	virtual void performAction() const {
		purgeNode(node, true);
	}
	
	static DeletionInfo purgeNode(OctreeNode::Ptr node, bool recursive) {
		// tells father to forget her...
		bool deleteBranch = false;
		do {
			BranchNode::Ptr bnp = dynamic_cast< BranchNode::Ptr >(node->getFather());
			deleteBranch = bnp->deleteChild(node->getChildIdx());
			
			// then free leaf's memory (no longer needed)
			delete node;
			
			// and prepare for upper level deletion (if needed)
			node = bnp;
		} while (deleteBranch && recursive && (!node->isRoot()));
		
		return DeletionInfo( deleteBranch && (!node->isRoot()),
				node
		);
	}
};

template < typename T >
class PurgeLeafTicket : public OctreeLeafTicket< T > {
	
private:
	typedef typename OctreeLeafTicket< T >::LeafPtr LeafPtr;
	
public:
	PurgeLeafTicket(const LeafPtr leaf) : OctreeLeafTicket< T >(leaf) { }
	
	virtual ~PurgeLeafTicket() { }
	
	virtual void performAction() const {
		purgeLeaf(OctreeLeafTicket< T >::getTarget(), true);
	}
	
	static PurgeNodeTicket::DeletionInfo purgeLeaf(LeafPtr leaf, bool recursive) {
		
		// adjust previous & next pointers
		if (leaf->getPrevious() != NULL) {
			leaf->getPrevious()->setNext(leaf->getNext());
		}
		if (leaf->getNext() != NULL) {
			leaf->getNext()->setPrevious(leaf->getPrevious());
		}
		
		return PurgeNodeTicket::purgeNode(leaf, recursive);
	}
	
};

template < typename T >
class UpdateDataTicket : public OctreeLeafTicket< T > {
	
public:
	typedef typename LeafNode< T >::DataConstPtr DataConstPtr;
	
private:
	typedef typename OctreeLeafTicket< T >::LeafPtr LeafPtr;
	
private:
	const DataConstPtr newData;
	
public:
	UpdateDataTicket(const LeafPtr leaf, const DataConstPtr &newData) :
		OctreeLeafTicket< T >(leaf), newData(newData) { }
	
	virtual ~UpdateDataTicket() { }
	
	virtual void performAction() const {
		updateData(OctreeLeafTicket< T >::getTarget(), newData);
	}
	
	static void updateData(LeafPtr leaf, const DataConstPtr &newData) {
		leaf->setData(newData);
	}
	
};

#endif /* OCTREE_TICKETS_HPP_ */
