/*
 * Octree.hpp
 *
 *  Created on: 22/ago/2012
 *      Author: socket
 */

#ifndef OCTREE_HPP_
#define OCTREE_HPP_

#include <ostream>
#include <deque>
#include <map>
#include <cmath>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>
#include <boost/ptr_container/ptr_deque.hpp>
#include <boost/assign/ptr_list_inserter.hpp>

#include <Eigen/Geometry>

#include "common/AtomicNumber.hpp"
#include "octree_nodes.hpp"
#include "octree_tickets.hpp"
#include "Voxel.hpp"


class SimpleBoxCache {
	
private:
	const Eigen::Vector3f EXTENT;
	std::map<u_int, SimpleBox::ConstPtr> CACHE;
	
public:
	SimpleBoxCache(Eigen::Vector3f extent) : EXTENT(extent), CACHE() {
		GeometryUtils::checkExtent(EXTENT);
	}
	
	virtual ~SimpleBoxCache() { }
	
	SimpleBox::ConstPtr getSimpleBox(u_int depth) {
		std::map<u_int, SimpleBox::ConstPtr>::const_iterator elm = CACHE.find(depth);
		
		SimpleBox::ConstPtr sbp;
		if(elm == CACHE.end()) {
			// element not found, build, insert & return
			float rate = std::pow((float)2.0, (float)depth);
			sbp = boost::make_shared< SimpleBox >(EXTENT / rate);
			
			CACHE[depth] = sbp;
		} else {
			sbp = elm->second;
		}
		
		return sbp;
	}
	
	ShiftedBox::ConstPtr getShiftedBox(u_int depth,
			const Eigen::Translation3f &shift = Eigen::Translation3f::Identity()) {
		
		return boost::make_shared< ShiftedBox >(getSimpleBox(depth), shift);
	}
};

template < typename LeafType >
class StoredData {
	
public:
	typedef std::pair< 
			ShiftedBox::ConstPtr,
			typename LeafType::DataConst > VoxelPair;
	typedef std::deque< VoxelPair > VoxelData;
	typedef boost::shared_ptr< VoxelData > VoxelDataPtr;
	
private:
	const VoxelDataPtr data;
	
public:
	StoredData(const VoxelDataPtr &data) : data(data) { }
	virtual ~StoredData() { }
	
	VoxelDataPtr getStoredData() const {
		return data;
	}
	
};


/**
 * Thread-safe implementation of the Octree data structure
 */
template <typename DataT, typename data_traits = DataTraits< DataT > >
class Octree {
	
public:
	typedef LeafNode< DataT, data_traits > LeafType;
	typedef typename LeafType::Ptr LeafPtr;
	typedef typename LeafType::ConstPtr LeafConstPtr;
	
	typedef typename LeafType::DataRef DataRef;
	typedef typename LeafType::DataConstRef DataConstRef;
	
	typedef BranchNode* BranchPtr;
	
	typedef typename SimpleBox::ConstPtr SimpleBoxPtr;
	
	typedef std::deque< LeafPtr > LeavesDeque;
	typedef boost::shared_ptr< std::deque< LeafPtr > > LeavesDequePtr;
	
	typedef boost::array< LeafPtr, BranchNode::N_CHILDREN > LeavesArray;
	typedef boost::shared_ptr< LeavesArray > LeavesArrayPtr;
	
	typedef StoredData< LeafType > DataView;
	
private:
	typedef OctreeTicket Ticket;
	typedef boost::ptr_deque< Ticket > TicketDeque;
	
	typedef boost::shared_lock< boost::shared_mutex > SharedLock;
	typedef boost::unique_lock< boost::shared_mutex > UniqueLock;
	typedef boost::lock_guard< boost::mutex > TicketLock;
	
	typedef AtomicNumber<u_int> Versioner;
	
private:
	
	const Eigen::Vector3f EXTENT;
	mutable SimpleBoxCache BOX_CACHE;
	const boost::shared_ptr< LeafType > LEAVES_LIST;
	BranchPtr ROOT;
	
	boost::mutex ticketMutex;
	TicketDeque ticketQueue;
	
	mutable boost::shared_mutex mutex;
	Versioner versioner;
	
public:
	
	Octree(Eigen::Vector3f extent) :
			EXTENT(extent),
			BOX_CACHE(extent),
			LEAVES_LIST(
					boost::make_shared< LeafType >(
							BOX_CACHE.getShiftedBox(0)
							)
						)
	{
		
		GeometryUtils::checkExtent(EXTENT);
		
		/* create a FAKE root, link it with the leaves list and then "push"
		 * it to get a branch (REAL root) and it's first children level
		 */
		boost::shared_ptr< LeafType > fakeRoot = 
				boost::make_shared< LeafType >(BOX_CACHE.getShiftedBox(0));
		
		PushedLevel level = createLevel(fakeRoot.get(), versioner.get());
		
		// then set the REAL root pointer & link it in the leaves list
		ROOT = level.newBranch;
		LEAVES_LIST->setNext(level.newLeaves->front());
		level.newLeaves->front()->setPrevious(LEAVES_LIST.get());
		
		notifyChanges();
	}
	
	virtual ~Octree() {
		notifyChanges();
		
		delete ROOT;
	}
	
	/**
	 * Invoke this method every time you make some changes to the
	 * data structure and want to persist them, that is make them available
	 * to any subsequent call of ::getStoredData().
	 */
	void notifyChanges() {
		
		/* the defer_lock here is used just for "teaching" in fact until lock
		 * acquisition order remains octree->ticket in the whole class, no
		 * deadlock is possible
		 * 
		 * Edit: defer_lock removed in order to use a simpler lock_guard on
		 * the ticketMutex; as stated before, DO NOT CHANGE locking order
		 * in the whole class!!
		 */
		UniqueLock octreeLock(mutex);
		TicketLock ticketLock(ticketMutex);
		
		while (!ticketQueue.empty()) {
			TicketDeque::auto_type ticket = ticketQueue.pop_back();
			ticket->performAction();
		}
		
		versioner.incAndGet();
	}
	
	
	/**
	 * Calling this method will invalidate given leaf pointer (the leaf
	 * will be deleted)
	 * @param leaf
	 * @return
	 */
	LeavesArrayPtr pushLevel(LeafPtr leaf) {
		
		SharedLock _(mutex);
		
		u_int currVersion = versioner.get();
		
		PushedLevel newLevel = createLevel(leaf, currVersion);
		
		if (isNewlyCreated(leaf, currVersion)) {
			/* this is a newly created leaf, so, we can make changes directly
			 * on it
			 */
			PushLevelTicket< LeafType >::attachBranch(leaf, newLevel.newBranch);
			
		} else {
			/* this is a leaf attached to the tree, so we have to create a
			 * ticket in order to perform required action
			 */
			TicketLock l(ticketMutex);
			boost::assign::ptr_push_back< PushLevelTicket < LeafType > >( ticketQueue )(leaf, newLevel.newBranch);
		}
		
		return newLevel.newLeaves;
	}
	
	void purgeLeaf(LeafPtr leaf) {
		
		SharedLock _(mutex);
		
		if (isNewlyCreated(leaf, versioner.get())) {
			PurgeNodeTicket::DeletionInfo info = 
					PurgeLeafTicket < LeafType >::purgeLeaf(leaf, false);
			
			if (info.shouldDelete) {
				TicketLock l(ticketMutex);
				boost::assign::ptr_push_back< PurgeNodeTicket >( ticketQueue )(info.reference);
			}
			
		} else {
			TicketLock l(ticketMutex);
			boost::assign::ptr_push_back< PurgeLeafTicket< LeafType > >( ticketQueue )(leaf);
		}
		
	}
	
	void updateLeafData(LeafPtr leaf, DataConstRef data) {
		
		SharedLock _(mutex);
		
		if(isNewlyCreated(leaf, versioner.get())) {
			UpdateDataTicket< LeafType >::updateData(leaf, data);
			
		} else {
			TicketLock l(ticketMutex);
			boost::assign::ptr_push_back< UpdateDataTicket< LeafType > >( ticketQueue )(leaf, data);
		}
		
	}
	
	/**
	 * 
	 * @param obb
	 * @param isom isometry transformation from this box (the MODEL) to given one:
	 * needed to transform \c obb coordinates in MODEL's basis
	 * @return
	 */
	LeavesDequePtr getIntersectingLeaves(const SimpleBox &obb,
			const Eigen::Isometry3f &isom, bool accurate) {
		
		SharedLock _(mutex);
		
		typedef std::deque< OctreeNode::Ptr > NodeQueue;
		typedef boost::shared_ptr< NodeQueue > NodeQueuePtr;
		
		NodeQueuePtr currLvlNodes = boost::make_shared< NodeQueue >();
		for (int i = 0; i < BranchNode::N_CHILDREN; i++) {
			if (getRoot()->hasChild(i))
				currLvlNodes->push_back(getRoot()->getChild(i));
		}
		
		LeavesDequePtr intersectingLeaves = boost::make_shared< LeavesDeque >();
		
		while (!currLvlNodes->empty()) {
			OctreeNode::Ptr currNode = currLvlNodes->back();
			currLvlNodes->pop_back();
			
			const ShiftedBox::ConstPtr &currBox = currNode->getBox();
			
			// checks for intersection
			if (currBox->isIntersecting(obb, isom, accurate)) {
				
				/* currNode is intersecting given box so now we have to
				 * try to expand it or save it as an intersecting leaf
				 */
				
				switch (currNode->getType()) {
					case OctreeNode::LEAF_NODE: {
						LeafPtr lnp = static_cast<LeafPtr>(currNode);
						intersectingLeaves->push_back(lnp);
						break;
					}
					case OctreeNode::BRANCH_NODE: {
						BranchPtr bnp = static_cast<BranchPtr>(currNode);
						// add children to nextLvlNodes
						for (int i = 0; i < BranchNode::N_CHILDREN; i++) {
							if (bnp->hasChild(i))
								currLvlNodes->push_back(bnp->getChild(i));
						}
						break;
					}
					default:
						throw std::runtime_error("invalid node type");
						break;
				}
			}
			
		}
		
		return intersectingLeaves;
	}
	
	/**
	 * Returns all data contained inside octree leafs: if you activate the
	 * onlyIfChanged flag, data will be returned only if octree state changed
	 * since last time the method was invoked with the flag on, otherwise
	 * an empty collection is returned.
	 * 
	 * @param onlyIfChanged
	 * @return
	 */
	DataView getStoredData(bool onlyIfChanged = false) const {
		
		typename DataView::VoxelDataPtr data = boost::make_shared< typename DataView::VoxelData >();
		
		SharedLock _(mutex);
		
		if (onlyIfChanged) {
			// TODO reset 'changed' flag with an atomic compare-exchange operation
		}
		
		LeafPtr currLeaf = getLeavesListHead();
		
		while((currLeaf = currLeaf->getNext()) != NULL) {
			typename DataView::VoxelPair vp(currLeaf->getBox(), currLeaf->getData());
			data->push_back(vp);
		}
		
		return data;
	}
	
	
private:
	
	inline
	BranchPtr getRoot() const {
		return ROOT;
	}
	
	inline
	LeafPtr getLeavesListHead() const {
		return LEAVES_LIST.get();
	}
	
	inline
	bool isNewlyCreated(const LeafPtr &leaf, u_int currVersion) const {
		return leaf->getVersion() >= currVersion;
	}
	
	
	struct PushedLevel {
		PushedLevel() : newLeaves(boost::make_shared< LeavesArray >()) { }
		
		BranchPtr newBranch;
		LeavesArrayPtr newLeaves;
	};
	
	/**
	 * Create a one-level tree detached from the main one; the new tree will
	 * have:
	 * \li inner-consistent: meaning that new branch and all leaves are linked
	 * together;
	 * \li father-consistent: that is new branch is linked also with
	 * correct father;
	 * \li leaves list-consistent meaning that first leaf's prev pointer and
	 * last leaf next pointer are left pointing to NULL;
	 * \li main tree isolation that is it will not modify main tree links, so
	 * it is not reachable from it.
	 * @param leaf
	 * @return
	 */
	PushedLevel createLevel(const LeafConstPtr &leaf, u_int leafVersion) const {
		
		PushedLevel toRet;
		
		// first create new branch node that will replace given leaf
		if (leaf->isRoot()) {
			toRet.newBranch = new BranchNode(leaf->getBox());
		} else {
			toRet.newBranch = new BranchNode(leaf->getFather(),
				leaf->getChildIdx(),
				leaf->getBox());
		}
		
		// create some variables needed in the following leaves-generation loop
		u_int newDepth = leaf->getDepth() + 1;
		LeafPtr prevNode = NULL;
		
		SimpleBox::ConstPtr thisLvlBox = BOX_CACHE.getSimpleBox(newDepth);
		ShiftedBox baseSBox = leaf->getBox()->getResized(thisLvlBox);
		// base translation needed to build all the others
		Eigen::Vector3f baseTraslation(thisLvlBox->getHalfExtent());
		
		for (u_char i = 0; i < BranchNode::N_CHILDREN; i++) {
			// calculate child traslation
			Eigen::Vector3f traslation(baseTraslation);
			
			switch (i) {
				case 0:
					traslation *= -1;
					break;
				case 1:
					traslation[1] *= -1;
					traslation[2] *= -1;
					break;
				case 2:
					traslation[2] *= -1;
					break;
				case 3:
					traslation[0] *= -1;
					traslation[2] *= -1;
					break;
				case 4:
					traslation[0] *= -1;
					traslation[1] *= -1;
					break;
				case 5:
					traslation[1] *= -1;
					break;
				case 6:
					// all positive
					break;
				case 7:
					traslation[0] *= -1;
					break;
					
				default:
					throw std::runtime_error("too much children");
					break;
			}
			
			// create shifted box for the child
			ShiftedBox newBox = baseSBox.getShifted(Eigen::Translation3f(traslation));
			
			LeafPtr child = new LeafType(
					toRet.newBranch,
					i,
					boost::make_shared< ShiftedBox >(newBox),
					newDepth, leafVersion);
			
			// set previous & next pointers
			child->setPrevious(prevNode);
			if (prevNode != NULL)
				prevNode->setNext(child);
			
			prevNode = child;
			
			toRet.newBranch->setChild(i, child);
			
			toRet.newLeaves->at(i) = child;
		}
		
		return toRet;
	}
	
	friend std::ostream & operator<<(std::ostream &os, const Octree &tree) {
		os << "Octree(extent=[" << tree.EXTENT.transpose() << "])" << "->"
				<< *tree.ROOT;
		
		return os;
	}
	
};

#endif /* OCTREE_HPP_ */
