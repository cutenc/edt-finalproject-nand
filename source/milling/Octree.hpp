/*
 * Octree.hpp
 *
 *  Created on: 22/ago/2012
 *      Author: socket
 */

#ifndef OCTREE_HPP_
#define OCTREE_HPP_

#include <queue>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <Eigen/Geometry>

#include "octree_nodes.hpp"

/**
 * Thread-safe implementation of the Octree data structure
 */
template <typename DataT>
class Octree {
	
	typedef LeafNode< DataT >* LeafNodePtr;
	typedef BranchNode* BranchNodePtr;
	
	const int MAX_DEPTH;
	const Eigen::Vector3d EXTENT;
	const boost::shared_ptr<BranchNode> ROOT;
	boost::shared_ptr< LeafNode< DataT > > leavesList;
	
public:
	
	Octree(int maxDepth, Eigen::Vector3d extent) :
			MAX_DEPTH(maxDepth),
			EXTENT(extent), 
			ROOT(boost::make_shared<BranchNode>()),
			leavesList(boost::make_shared< LeafNode< DataT > >(ROOT.get(), 0, Eigen::Vector3d::Zero(), -1)) {
		
		GeometryUtils::checkExtent(extent);
		if (MAX_DEPTH <= 1)
			throw std::invalid_argument("MAX_DEPTH must be > 1");
		
		LeafNodePtr prevNode = leavesList.get();
		
		// create root children
		for (int i = 0; i < BranchNode::N_CHILDREN; i++) {
			// calculate child traslation
			Eigen::Vector3d traslation(EXTENT / 2.0);
			
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
					break;
			}
			
			traslation[0] *= (-1) * ((i & 0x01) | (i & 0x02));
			traslation[1] *= (-1) * (i & 0x02);
			traslation[2] *= (-1) * (i & 0x04);
			
			Eigen::Vector3d totTraslation = ROOT->getTraslation() + traslation;
			
			LeafNodePtr child = new LeafNode< DataT >(ROOT.get(), i, totTraslation, 1);
			child->setPrevious(prevNode);
			prevNode = child;
			
			ROOT->setChild(i, child);
		}
	}
	
	virtual ~Octree() { }
	
	/**
	 * Returns all data contained inside octree leafs: if you activate the
	 * onlyIfChanged flag, data will be returned only if octree state changed
	 * since last time the method was invoked with the flag on, otherwise
	 * an empty std::vector is returned.
	 * 
	 * @param onlyIfChanged
	 * @return
	 */
	std::vector<DataT> getStoredData(bool onlyIfChanged = false);
	
	/**
	 * Invoke this method every time you make some changes to the
	 * data structure and want to persist them, that is make them available
	 * to any subsequent call of ::getStoredData().
	 */
	void notifyChanges();
	
//	std::vector< LeafNodePtr > pushLevel(LeafNodePtr leaf) {
//		
//		BranchNode *branch = new BranchNode(leaf->getFather(), leaf->getChildIdx());
//		
//		/* devo ricordarmi che queste modifiche non vanno effettuate finchè
//		 * non viene invocato il metodo notifyChanges => il vecchio BranchNode
//		 * deve rimanere non-cosciente del fatto che la vecchia foglia è stata
//		 * sostituita da un nuovo BranchNode fino a quel momento
//		 * Probabilmente sarà necessario prevedere una lista di "cambiamenti
//		 * da fare"
//		 */
//		
//		
//	}
	
	typedef boost::shared_ptr< std::vector< LeafNodePtr > > LeavesVectorPtr;
	
	/**
	 * 
	 * @param obb
	 * @param traslation from this box center to given box center
	 * @param rotation to given box basis
	 * @return
	 */
	LeavesVectorPtr getIntersectingLeaves(const SimpleBox &obb,
			const Eigen::Vector3d &traslation,
			const Eigen::Matrix3d &rotation) const {
		
		typedef boost::shared_ptr< std::queue< OctreeNodePtr > > NodeQueuePtr;
		
		NodeQueuePtr currLvlNodes = boost::make_shared< std::queue< OctreeNodePtr > >();
		for (int i = 0; i < BranchNode::N_CHILDREN; i++) {
			currLvlNodes->push(ROOT->getChild(i));
		}
		
		LeavesVectorPtr intersectingLeaves = boost::make_shared< std::vector< LeafNodePtr > >();
		
		// used for caching current level SimpleBox
		boost::shared_ptr<SimpleBox> currBox = boost::make_shared< SimpleBox >(EXTENT / 2.0);
		u_int currDepth = 1;
		
		NodeQueuePtr nextLvlNodes = boost::make_shared< std::queue< OctreeNodePtr > >();
		
		do {
			OctreeNodePtr currNode = currLvlNodes->front();
			currLvlNodes->pop();
			
			Eigen::Vector3d currTraslation = traslation - currNode->getTraslation();
			
			// checks for intersection
			if (currBox->isIntersecting(obb, currTraslation, rotation)) {
				
				/* currNode is intersecting given box so now we have to
				 * try to expand it ora save it as an intersecting leaf
				 */
				
				switch (currNode->getType()) {
					case LEAF_NODE: {
						LeafNodePtr lnp = dynamic_cast<LeafNodePtr>(currNode);
						intersectingLeaves->push_back(lnp);
						break;
					}
					case BRANCH_NODE: {
						BranchNodePtr bnp = dynamic_cast<BranchNodePtr>(currNode);
						// add children to nextLvlNodes
						for (int i = 0; i < BranchNode::N_CHILDREN; i++) {
							nextLvlNodes->push(bnp->getChild(i));
						}
						break;
					}
					default:
						throw std::runtime_error("invalid node type");
						break;
				}
			}
			
			
			if (currLvlNodes->empty()) {
				// finished checking current depth, switch to next level
				currLvlNodes = nextLvlNodes;
				nextLvlNodes = boost::make_shared< std::queue< OctreeNodePtr > >();
				
				currDepth++;
				currBox = boost::make_shared< SimpleBox >(currBox->getHalfExtent());
			}
			
		} while (!currLvlNodes->empty());
		
		return intersectingLeaves;
	}
};

#endif /* OCTREE_HPP_ */
