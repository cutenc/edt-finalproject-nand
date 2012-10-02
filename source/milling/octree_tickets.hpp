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
	
protected:
	static void checkNull(const void * p) throw(std::invalid_argument) {
		if (p == NULL)
			throw std::invalid_argument("NULL pointer given");
	}
	
};

template < typename LeafType >
class OctreeLeafTicket : public OctreeTicket {
	
protected:
	typedef typename LeafType::Ptr LeafPtr;
	
	const LeafPtr targetLeaf;
	
public:
	OctreeLeafTicket(const LeafPtr &leaf) : targetLeaf(leaf) {
		checkNull(targetLeaf);
	}
	
	virtual ~OctreeLeafTicket() { }
	
	LeafPtr getTarget() const { return this->targetLeaf; }
	
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
			BranchNode::Ptr bnp = static_cast< BranchNode::Ptr >(node->getFather());
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




template < typename LeafType >
class PurgeLeafTicket : public OctreeLeafTicket< LeafType > {
	
private:
	typedef typename OctreeLeafTicket< LeafType >::LeafPtr LeafPtr;
	
public:
	PurgeLeafTicket(const LeafPtr leaf) : OctreeLeafTicket< LeafType >(leaf) { }
	
	virtual ~PurgeLeafTicket() { }
	
	virtual void performAction() const {
		purgeLeaf(OctreeLeafTicket< LeafType >::getTarget(), true);
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




template < typename LeafType >
class PushLevelTicket : public OctreeLeafTicket< LeafType > {
	
private:
	typedef typename OctreeLeafTicket< LeafType >::LeafPtr LeafPtr;
	
	BranchNode::Ptr newBranch;
	
public:
	PushLevelTicket(const LeafPtr leaf, const BranchNode::Ptr &newBranch) :
		OctreeLeafTicket< LeafType >(leaf), newBranch(newBranch) {
		
		OctreeTicket::checkNull(newBranch);
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
		OctreeTicket::checkNull(newBranch);
		
		BranchNode::Ptr father = static_cast< BranchNode::Ptr >(leaf->getFather());
		assert(father == newBranch->getFather());
		
		LeafPtr firstLeaf = static_cast< LeafPtr >(newBranch->getFirst(OctreeNode::LEAF_NODE)),
				lastLeaf = static_cast< LeafPtr >(newBranch->getLast(OctreeNode::LEAF_NODE));
		
		// sanity check assertion
		assert(firstLeaf != NULL && lastLeaf != NULL);
		/* failing this assertion means that a voxel has been split because
		 * it was intersecting-not-contained but children processing
		 * discovers that they are all inside. From a mathematical point
		 * of view this is an absurd but it may happen due to float/double
		 * approximation errors. To take into account such errors we
		 * introduce a "truncation" in distance calculation trying to make
		 * impossible that such an event happens.
		 */
		
		
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
		leaf->setData(newData);
	}
	
};

#endif /* OCTREE_TICKETS_HPP_ */
