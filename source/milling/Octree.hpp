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

#include "octree_nodes.hpp"
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
	
	ShiftedBox::ConstPtr getShiftedBox(u_int depth, const Eigen::Translation3d &shift) const {
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

class Octree {
	
public:
	typedef typename LeafNode::Ptr LeafPtr;
	typedef typename LeafNode::ConstPtr LeafConstPtr;
	
	typedef BranchNode::Ptr BranchPtr;
	typedef BranchNode::ConstPtr BranchConstPtr;
	
	typedef typename SimpleBox::ConstPtr SimpleBoxPtr;
	
	typedef OctreeNode::VersionInfo VersionInfo;
	
private:
	
	const Eigen::Vector3d EXTENT;
	const u_int MAX_DEPTH;
	const SimpleBoxCache BOX_CACHE;
	BranchPtr ROOT;
	
public:
	
	Octree(Eigen::Vector3d extent, u_int maxDepth) :
			EXTENT(extent),
			MAX_DEPTH(maxDepth),
			BOX_CACHE(extent, maxDepth)
	{
		
		GeometryUtils::checkExtent(EXTENT);
		
		/* create a FAKE root, link it with the leaves list and then "push"
		 * it to get a branch (REAL root) and it's first children level
		 */
		VersionInfo fakeVinfo(1, 1);
		boost::shared_ptr< LeafNode > fakeRoot = 
				boost::make_shared< LeafNode >(BOX_CACHE.getShiftedBox(0,
						Eigen::Translation3d::Identity()), fakeVinfo);
		
		ROOT = createLevel(fakeRoot.get(), fakeVinfo);
	}
	
	virtual ~Octree() {
		delete ROOT;
	}
	
	
	
	inline
	BranchPtr getRoot() const {
		return ROOT;
	}
	
	void updateData(LeafPtr lpt, const VersionInfo &vinfo) {
		lpt->setFirstChangeVersion(vinfo);
	}
	
	void deleteLeaf(LeafPtr &lpt) {
		deleteNode(lpt);
	}
	
	void deleteBranch(BranchPtr &bpt) {
		// I have to signal nothing when an internal node is deleted
		deleteNode(bpt);
	}
	
	BranchNode::Ptr pushLeaf(LeafPtr &lpt, const VersionInfo &vinfo) {
		
		lpt->getFather()->setFirstChangeVersion(vinfo);
		
		BranchPtr newBranch = createLevel(lpt, vinfo);
		
		// now attach branch to the tree
		BranchNode::Ptr father = static_cast< BranchNode::Ptr >(lpt->getFather());
		assert(father == newBranch->getFather());
		
		u_char leafIdx = lpt->getChildIdx();
		
		// then delete leaf from father
		father->deleteChild(leafIdx);
		
		// then set newBranch as father's child
		father->setChild(leafIdx, newBranch);
		
		// then free leaf's memory (no longer needed)
		delete lpt; lpt = NULL;
		
		return newBranch;
	}
	
private:
	void deleteNode(OctreeNode::Ptr node) {
		// tells father to forget its children
		BranchNode::Ptr bnp = static_cast< BranchNode::Ptr >(node->getFather());
		bnp->deleteChild(node->getChildIdx());
		
		// then free leaf's memory (no longer needed)
		delete node;
	}
	
	/**
	 * Create a one-level tree detached from the main one; the new tree will
	 * have:
	 * \li inner-consistent: meaning that new branch and all leaves are linked
	 * together;
	 * \li father-consistent: that is new branch is linked also with
	 * correct father;
	 * \li main tree isolation that is it will not modify main tree links, so
	 * it is not reachable from it.
	 * @param leaf
	 * @return
	 */
	BranchPtr createLevel(const LeafConstPtr &leaf, const VersionInfo &vinfo) const {
		
		BranchPtr newBranch;
		// first create new branch node that will replace given leaf
		if (leaf->isRoot()) {
			newBranch = new BranchNode(leaf->getBox(), vinfo);
		} else {
			newBranch = new BranchNode(leaf->getFather(),
				leaf->getChildIdx(),
				leaf->getBox(),
				vinfo);
		}
		
		// create some variables needed in the following leaves-generation loop
		u_int newDepth = leaf->getDepth() + 1;
		
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
			ShiftedBox newBox = baseSBox.getShifted(Eigen::Translation3d(traslation));
			
			LeafPtr child = new LeafNode(
					newBranch,
					i,
					boost::make_shared< ShiftedBox >(newBox),
					vinfo);
			
			newBranch->setChild(i, child);
		}
		
		return newBranch;
	}
	
	friend std::ostream & operator<<(std::ostream &os, const Octree &tree) {
		os << "Octree(extent=[" << tree.EXTENT.transpose() << "])" << "->"
				<< *tree.ROOT;
		
		return os;
	}
	
};

#endif /* OCTREE_HPP_ */
