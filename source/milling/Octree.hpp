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
#include <boost/math/special_functions.hpp>
#include <boost/function.hpp>

#include <Eigen/Geometry>

#include "common/AtomicNumber.hpp"
#include "octree_nodes.hpp"
#include "octree_tickets.hpp"
#include "Voxel.hpp"


class SimpleBoxCache {
	
private:
	typedef std::map<u_int, SimpleBox::ConstPtr> CacheMap;
	typedef boost::shared_ptr< CacheMap > CacheMapPtr;
	typedef boost::shared_ptr< const CacheMap > CacheMapConstPtr;
	
	const u_int MAX_DEPTH;
	const CacheMapConstPtr CACHE;
	
public:
	SimpleBoxCache(const Eigen::Vector3d &extent, u_int maxDepth) :
		MAX_DEPTH(maxDepth), CACHE(buildCache(extent, maxDepth))
	{
		GeometryUtils::checkExtent(extent);
	}
	
	virtual ~SimpleBoxCache() { }
	
	SimpleBox::ConstPtr getSimpleBox(u_int depth) const {
		assert(depth <= MAX_DEPTH);
		
		std::map<u_int, SimpleBox::ConstPtr>::const_iterator elm = CACHE->find(depth);
		return elm->second;
	}
	
	ShiftedBox::ConstPtr getShiftedBox(u_int depth,
			const Eigen::Translation3d &shift = Eigen::Translation3d::Identity()) const {
		
		return boost::make_shared< ShiftedBox >(getSimpleBox(depth), shift);
	}
	
private:
	static CacheMapPtr buildCache(const Eigen::Vector3d &extent, u_int maxDepth) {
		
		CacheMapPtr cache = boost::make_shared< CacheMap >();
		/* build cache: it should not use too much memory because
		 * usually max depths are some tens, moreover we cannot afford a lazy
		 * initialization of each simple-box due to problems in the C++
		 * memory model (in fact there's no memory model in current C++
		 * specification) that can cause a lot of troubles and unespected
		 * behaviours. For more informations see
		 * http://www.cs.umd.edu/~pugh/java/memoryModel/DoubleCheckedLocking.html
		 */
		for (u_int i = 0; i <= maxDepth; ++i) {
			/* used pow function and not bit shift operator because this way
			 * i may go beyond maxDepth = 32 or 64.
			 */
			double rate = pow(2.0, (double)i);
			(*cache)[i] = boost::make_shared< SimpleBox >(extent / rate);
		}
		
		return cache;
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
 * Implementation of the Octree data structure.
 * This implementation permits a bit of concurrency in that a thread polling
 * #getStoredData(bool) method will always optain a consistent state of the
 * Octree even if another thread is modifying it throug method
 * #processIntersectingLeaves. After any change made to the tree the method
 * #notifyChanges should be called
 */
template <typename DataT, typename data_traits = DataTraits< DataT > >
class Octree {
	
public:
	typedef LeafNode< DataT, data_traits > LeafType;
	typedef typename LeafType::Ptr LeafPtr;
	typedef typename LeafType::ConstPtr LeafConstPtr;
	
	typedef typename LeafType::DataRef DataRef;
	typedef typename LeafType::DataConstRef DataConstRef;
	
	typedef BranchNode::Ptr BranchPtr;
	typedef BranchNode::ConstPtr BranchConstPtr;
	
	typedef typename SimpleBox::ConstPtr SimpleBoxPtr;
	
	typedef std::deque< LeafPtr > LeavesDeque;
	typedef boost::shared_ptr< std::deque< LeafPtr > > LeavesDequePtr;
	
	typedef boost::array< LeafPtr, BranchNode::N_CHILDREN > LeavesArray;
	typedef boost::shared_ptr< LeavesArray > LeavesArrayPtr;
	
	typedef StoredData< LeafType > DataView;
	
	typedef boost::function< bool (OctreeNode::ConstPtr) > BranchChoserFunction;
	typedef boost::function< void (LeafPtr) > LeafProcesserFunction;
	
private:
	typedef OctreeTicket Ticket;
	typedef boost::ptr_deque< Ticket > TicketDeque;
	
	typedef boost::lock_guard< boost::mutex > NotifyLock;
	typedef boost::lock_guard< boost::mutex > TicketLock;
	
	typedef AtomicNumber<u_int> Versioner;
	
private:
	
	const Eigen::Vector3d EXTENT;
	const u_int MAX_DEPTH;
	const SimpleBoxCache BOX_CACHE;
	const boost::shared_ptr< LeafType > LEAVES_LIST;
	BranchPtr ROOT;
	
	boost::mutex ticketMutex;
	TicketDeque ticketQueue;
	
	mutable boost::mutex mutex;
	Versioner versioner;
	
public:
	
	Octree(Eigen::Vector3d extent, u_int maxDepth) :
			EXTENT(extent),
			MAX_DEPTH(maxDepth),
			BOX_CACHE(extent, maxDepth),
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
	 * Calling this method will invalidate given leaf pointer (the leaf
	 * will be deleted)
	 * @param leaf
	 * @return
	 */
	LeavesArrayPtr pushLevel(LeafPtr leaf) {
		assert(leaf->getDepth() < MAX_DEPTH);
		
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
		
		if(isNewlyCreated(leaf, versioner.get())) {
			UpdateDataTicket< LeafType >::updateData(leaf, data);
			
		} else {
			TicketLock l(ticketMutex);
			boost::assign::ptr_push_back< UpdateDataTicket< LeafType > >( ticketQueue )(leaf, data);
		}
		
	}
	
	void processTree(BranchChoserFunction branchChoser,
			LeafProcesserFunction leafProcesser,
			bool testLeaves) {
		
		BranchConstPtr root = this->getRoot();
		if (branchChoser(root)) {
			processTreeRecursive(root, branchChoser, leafProcesser, testLeaves);
		}
		
		/* notify even if root has not been chosen because
		 * branchChoser may have created some tickets
		 */ 
		this->notifyChanges();
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
		
		NotifyLock lock(mutex);
		
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
	
	
	void processTreeRecursive(const BranchConstPtr &branch,
			const BranchChoserFunction &branchChoser,
			const LeafProcesserFunction &leafProcesser,
			bool testLeaves) {
		
		for(u_char c = 0; c < BranchNode::N_CHILDREN; ++c) {
			if (!branch->hasChild(c)) {
				continue;
			}
			
			OctreeNode::Ptr child = branch->getChild(c);
			switch (child->getType()) {
				case OctreeNode::BRANCH_NODE:
				{
					BranchConstPtr bpt = static_cast< BranchConstPtr >(child);
					if (branchChoser(bpt)) {
						processTreeRecursive(bpt, branchChoser, leafProcesser, testLeaves);
					}
				}
					break;
				case OctreeNode::LEAF_NODE:
				{
					if (testLeaves && !branchChoser(child)) {
						/* i have to test leaf before performing processing,
						 * but test result told us not to parse leaf -> skip
						 * processing.
						 */
						continue;
					}
					
					LeafPtr lpt = static_cast< LeafPtr >(child);
					leafProcesser(lpt);
				}
					break;
				default:
					throw std::runtime_error("node type not registered");
					break;
			}
		}
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
		NotifyLock octreeLock(mutex);
		TicketLock ticketLock(ticketMutex);
		
		while (!ticketQueue.empty()) {
			TicketDeque::auto_type ticket = ticketQueue.pop_back();
			ticket->performAction();
		}
		
		versioner.incAndGet();
	}

	
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
		Eigen::Vector3d baseTraslation(thisLvlBox->getHalfExtent());
		
		for (u_char i = 0; i < BranchNode::N_CHILDREN; i++) {
			// calculate child traslation
			Eigen::Vector3d traslation(baseTraslation);
			
			switch (i) {
				case 0:
					traslation *= -1;
					break;
				case 1:
					traslation[1] = boost::math::changesign(traslation[1]);
					traslation[2] = boost::math::changesign(traslation[2]);
					break;
				case 2:
					traslation[2] = boost::math::changesign(traslation[2]);
					break;
				case 3:
					traslation[0] = boost::math::changesign(traslation[0]);
					traslation[2] = boost::math::changesign(traslation[2]);
					break;
				case 4:
					traslation[0] = boost::math::changesign(traslation[0]);
					traslation[1] = boost::math::changesign(traslation[1]);
					break;
				case 5:
					traslation[1] = boost::math::changesign(traslation[1]);
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
			ShiftedBox newBox = baseSBox.getShifted(Eigen::Translation3d(traslation));
			
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
