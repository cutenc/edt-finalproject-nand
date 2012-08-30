/*
 * Octree.hpp
 *
 *  Created on: 22/ago/2012
 *      Author: socket
 */

#ifndef OCTREE_HPP_
#define OCTREE_HPP_

#include <queue>
#include <deque>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <Eigen/Geometry>

#include "octree_nodes.hpp"

/**
 * Thread-safe implementation of the Octree data structure
 */
template <typename DataT>
class Octree {
	
	const static u_char ROOT_CHILD_IDX = 0; 
	
	const Eigen::Vector3d EXTENT;
	boost::shared_ptr<BranchNode> FAKE_ROOT;
	boost::shared_ptr< LeafNode< DataT > > LEAVES_LIST;
	
public:
	
	typedef LeafNode< DataT >* LeafPtr;
	typedef BranchNode* BranchPtr;
	
	Octree(Eigen::Vector3d extent) :
			EXTENT(extent), 
			FAKE_ROOT(boost::make_shared<BranchNode>()),
			LEAVES_LIST(boost::make_shared< LeafNode< DataT > >()) {
		
		GeometryUtils::checkExtent(EXTENT);
		
		// create REAL root
		LeafPtr realRoot = new LeafNode<DataT>(FAKE_ROOT.get(), Octree::ROOT_CHILD_IDX,
				Eigen::Vector3d::Zero(), 0);
		
		// link it with FAKE_ROOT and LEAVES_LIST
		FAKE_ROOT->setChild(Octree::ROOT_CHILD_IDX, realRoot);
		LEAVES_LIST->setNext(realRoot);
		realRoot->setPrevious(LEAVES_LIST.get());
		
		// start with depth 1
		pushLevel(realRoot);
		notifyChanges();
		
	}
	
	virtual ~Octree() { }
	
	/**
	 * Invoke this method every time you make some changes to the
	 * data structure and want to persist them, that is make them available
	 * to any subsequent call of ::getStoredData().
	 */
	void notifyChanges() {
		
		// TODO acquire writeLock
		
		// TODO implement NOTIFY changes
		
		// TODO release writeLock
	}
	
	typedef boost::shared_ptr< std::vector< LeafPtr > > LeavesVectorPtr;
	
	LeavesVectorPtr pushLevel(LeafPtr leaf) {
		
		// TODO acquire readLock
		
		// first create new branch node...
		BranchPtr branch = new BranchNode(leaf->getFather(),
				leaf->getChildIdx(),
				leaf->getTraslation());
		
		// ... then generate new children
		LeavesVectorPtr newLeaves = boost::make_shared< std::vector< LeafPtr > >();
		
		LeafPtr prevNode = leaf->getPrevious();
		Eigen::Vector3d baseTraslation(leaf->getSimpleBox(EXTENT).getHalfExtent());
		
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
			
			Eigen::Vector3d totTraslation = leaf->getTraslation() + traslation;
			
			LeafPtr child = new LeafNode< DataT >(branch, i,
					totTraslation, leaf->getDepth() + 1);
			
			prevNode->setNext(child);
			child->setPrevious(prevNode);
			prevNode = child;
			
			branch->setChild(i, child);
			
			newLeaves->push_back(child);
		}
		
		// ...adjust last child's next pointer and leaf->getNext prev pointer...
		newLeaves->back()->setNext(leaf->getNext());
		if (leaf->getNext() != NULL)
			leaf->getNext()->setPrevious(newLeaves->back());
		
		
		// ... then link new branch to remaining tree 
		/* TODO devo ricordarmi che queste modifiche non vanno effettuate finchè
		 * non viene invocato il metodo notifyChanges => il vecchio BranchNode
		 * deve rimanere non-cosciente del fatto che la vecchia foglia è stata
		 * sostituita da un nuovo BranchNode fino a quel momento
		 * Probabilmente sarà necessario prevedere una lista di "cambiamenti
		 * da fare"
		 */
		BranchPtr bnp = dynamic_cast<BranchPtr>(leaf->getFather());
		bnp->setChild(branch->getChildIdx(), branch);
		
		// TODO release readLock
		
		return newLeaves;
	}
	
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
		
		// TODO acquire ReadLock
		
		typedef boost::shared_ptr< std::queue< OctreeNodePtr > > NodeQueuePtr;
		
		NodeQueuePtr currLvlNodes = boost::make_shared< std::queue< OctreeNodePtr > >();
		for (int i = 0; i < BranchNode::N_CHILDREN; i++) {
			currLvlNodes->push(getRoot()->getChild(i));
		}
		
		LeavesVectorPtr intersectingLeaves = boost::make_shared< std::vector< LeafPtr > >();
		
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
						LeafPtr lnp = dynamic_cast<LeafPtr>(currNode);
						intersectingLeaves->push_back(lnp);
						break;
					}
					case BRANCH_NODE: {
						BranchPtr bnp = dynamic_cast<BranchPtr>(currNode);
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
		
		// TODO release readLock
		
		return intersectingLeaves;
	}
	
	
	typedef boost::shared_ptr< std::deque<DataT> > DataDeque;
	
	/**
	 * Returns all data contained inside octree leafs: if you activate the
	 * onlyIfChanged flag, data will be returned only if octree state changed
	 * since last time the method was invoked with the flag on, otherwise
	 * an empty std::vector is returned.
	 * 
	 * @param onlyIfChanged
	 * @return
	 */
	DataDeque getStoredData(bool onlyIfChanged = false) const {
		// TODO acquire readLock
		
		DataDeque data = boost::make_shared< std::deque< DataT > >();
		
		if (onlyIfChanged) {
			// TODO reset 'changed' flag with an atomic compare-exchange operation
		}
		
		// TODO implement THIS (now is only used to check if leaf queue is consistent
		LeafPtr currLeaf = getFirstLeaf();
		do {
			data->push_back(currLeaf->getData());
		} while((currLeaf = currLeaf->getNext()) != NULL);
		
		// TODO release ReadLock
		return data;
	}
	
	
private:
	
	BranchPtr getRoot() const {
		return dynamic_cast<BranchPtr>(this->FAKE_ROOT->getChild(Octree::ROOT_CHILD_IDX));
	}
	
	LeafPtr getFirstLeaf() const {
		return LEAVES_LIST->getNext();
	}
	
};

#endif /* OCTREE_HPP_ */
