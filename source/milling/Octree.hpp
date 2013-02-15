/**
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

/**
 * @class Octree
 *
 * defines an octree and the operations it can perform
 */
class Octree {
	
public:
	typedef LeafNode::Ptr LeafPtr;
	typedef LeafNode::ConstPtr LeafConstPtr;
	
	typedef BranchNode::Ptr BranchPtr;
	typedef BranchNode::ConstPtr BranchConstPtr;
	
	typedef OctreeNode::VersionInfo VersionInfo;
	
private:
	
	const Eigen::Vector3d EXTENT;
	BranchPtr ROOT;
	
public:
	
	/**
	 * constructor
	 * @param extent
	 */
	Octree(Eigen::Vector3d extent) :
			EXTENT(extent)
	{
		
		GeometryUtils::checkExtent(EXTENT);
		
		/* create a FAKE root, link it with the leaves list and then "push"
		 * it to get a branch (REAL root) and it's first children level
		 */
		VersionInfo fakeVinfo(1, 1);
		
		ShiftedBox::Ptr initialBox(new ShiftedBox(
				Eigen::Vector3d::Zero(),
				EXTENT
		));
		
		boost::shared_ptr< LeafNode > fakeRoot = boost::make_shared< LeafNode >(
				initialBox, fakeVinfo);
		
		ROOT = createLevel(fakeRoot.get(), fakeVinfo);
	}
	
	/**
	 * destructor
	 */
	virtual ~Octree() {
		delete ROOT;
	}
	
	
	/**
	 *
	 * @return pointer to root node
	 */
	inline
	BranchPtr getRoot() const {
		return ROOT;
	}
	
	/**
	 * updates the content of a leaf
	 * @param lpt
	 * @param vinfo
	 */
	void updateData(LeafPtr lpt, const VersionInfo &vinfo) {
		lpt->setFirstChangeVersion(vinfo);
	}
	
	/**
	 * deletes selected leaf
	 * @param lpt
	 */
	void deleteLeaf(LeafPtr &lpt) {
		deleteNode(lpt);
	}
	
	/**
	 * deletes a whole branch of the octree
	 *
	 * @param bpt
	 */
	void deleteBranch(BranchPtr &bpt) {
		// I have to signal nothing when an internal node is deleted
		deleteNode(bpt);
	}
	
	/**
	 * expands a leaf into 8 branches
	 *
	 * @param lpt
	 * @param vinfo
	 * @return
	 */
	BranchNode::Ptr pushLeaf(LeafPtr &lpt, const VersionInfo &vinfo) {
		
		lpt->getFather()->setFirstChangeVersion(vinfo);
		
		BranchPtr newBranch = createLevel(lpt, vinfo);
		
		// now attach branch to the tree
		BranchNode::Ptr father = static_cast< BranchNode::Ptr >(lpt->getFather());
		assert(father == newBranch->getFather());
		
		unsigned char leafIdx = lpt->getChildIdx();
		
		// then delete leaf from father
		father->deleteChild(leafIdx);
		
		// then set newBranch as father's child
		father->setChild(leafIdx, newBranch);
		
		// then free leaf's memory (no longer needed)
		delete lpt; lpt = NULL;
		
		return newBranch;
	}
	
private:
	
	/**
	 * deletes a generic node, plus all of its children, if any
	 *
	 * @param node
	 */
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
		
		/* we have to create new shift boxes for each child
		 * thanks to "OpenSceneGraph3 Cookbook" for the if-free code
		 */
		int s[3];
		Eigen::Vector3d extentSet[3] = {
			leaf->getBox()->getMin(),
			leaf->getBox()->getShift(),
			leaf->getBox()->getMax()
		};
		int childIdx = 0;
		
		for ( s[0]=0; s[0]<2; ++s[0] ) {
			for ( s[1]=0; s[1]<2; ++s[1] ) {
				for ( s[2]=0; s[2]<2; ++s[2] ) {
					
					// create shifted box for the child
					ShiftedBox::Ptr newBox(new ShiftedBox());
					ShiftedBox::MinMaxMatrix &minMax = newBox->getMatrix();
					
					for ( int a=0; a<3; ++a ) {
						minMax(a, ShiftedBox::MIN_IDX) = (extentSet[s[a] + 0])[a]; // min
						minMax(a, ShiftedBox::MAX_IDX) = (extentSet[s[a] + 1])[a]; // max
					}
					
					newBox->calculateExtentsAndVolume();
					
					// build child idx
					LeafPtr child = new LeafNode(
							newBranch,
							childIdx,
							newBox,
							vinfo);
					
					newBranch->setChild(childIdx, child);
					
					childIdx++;
				}
			}
		}
		
		return newBranch;
	}
	
	/**
	 * overrides <<
	 * @param os
	 * @param tree
	 * @return
	 */
	friend std::ostream & operator<<(std::ostream &os, const Octree &tree) {
		os << "Octree(extent=[" << tree.EXTENT.transpose() << "])" << "->"
				<< *tree.ROOT;
		
		return os;
	}
	
};

#endif /* OCTREE_HPP_ */
