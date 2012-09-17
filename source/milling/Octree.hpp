/*
 * Octree.hpp
 *
 *  Created on: 22/ago/2012
 *      Author: socket
 */

#ifndef OCTREE_HPP_
#define OCTREE_HPP_

#include <ostream>
#include <queue>
#include <deque>
#include <map>
#include <cmath>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/array.hpp>

#include <Eigen/Geometry>

#include "octree_nodes.hpp"
#include "Voxel.hpp"


class SimpleBoxCache {
	
	const Eigen::Vector3d EXTENT;
	std::map<u_int, SimpleBox::ConstPtr> CACHE;
	
public:
	SimpleBoxCache(Eigen::Vector3d extent) : EXTENT(extent), CACHE() {
		GeometryUtils::checkExtent(EXTENT);
	}
	
	virtual ~SimpleBoxCache() { }
	
	SimpleBox::ConstPtr getSimpleBox(u_int depth) {
		std::map<u_int, SimpleBox::ConstPtr>::const_iterator elm = CACHE.find(depth);
		
		SimpleBox::ConstPtr sbp;
		if(elm == CACHE.end()) {
			// element not found, build, insert & return
			double rate = std::pow(2.0, (double)depth);
			sbp = boost::make_shared< SimpleBox >(EXTENT / rate);
			
			CACHE[depth] = sbp;
		} else {
			sbp = elm->second;
		}
		
		return sbp;
	}
	
	ShiftedBox::ConstPtr getShiftedBox(u_int depth,
			const Eigen::Translation3d &shift = Eigen::Translation3d::Identity()) {
		
		return boost::make_shared< ShiftedBox >(getSimpleBox(depth), shift);
	}
};



/**
 * Thread-safe implementation of the Octree data structure
 */
template <typename DataT>
class Octree {
	
public:
	typedef typename LeafNode< DataT >::LeafPtr LeafPtr;
	typedef typename LeafNode< DataT >::LeafConstPtr LeafConstPtr;
	typedef typename LeafNode< DataT >::DataPtr DataPtr;
	typedef typename LeafNode< DataT >::DataConstPtr DataConstPtr;
	typedef BranchNode* BranchPtr;
	typedef typename SimpleBox::ConstPtr SimpleBoxPtr;
	
	typedef std::deque< LeafPtr > LeavesDeque;
	typedef boost::shared_ptr< std::deque< LeafPtr > > LeavesDequePtr;
	
	typedef boost::array< LeafPtr, BranchNode::N_CHILDREN > LeavesArray;
	typedef boost::shared_ptr< LeavesArray > LeavesArrayPtr;
	
	typedef std::deque< DataConstPtr > DataDeque;
	typedef boost::shared_ptr< DataDeque > DataDequePtr;
	
private:
	
	const Eigen::Vector3d EXTENT;
	SimpleBoxCache BOX_CACHE;
	const boost::shared_ptr< LeafNode< DataT > > LEAVES_LIST;
	BranchPtr ROOT;
	
public:
	
	Octree(Eigen::Vector3d extent) :
			EXTENT(extent),
			BOX_CACHE(extent),
			LEAVES_LIST(
					boost::make_shared< LeafNode< DataT > >(
							BOX_CACHE.getShiftedBox(0)
							)
						)
	{
		
		GeometryUtils::checkExtent(EXTENT);
		
		/* create a FAKE root, link it with the leaves list and then "push"
		 * it to get a branch (REAL root) and it's first children level
		 */
		boost::shared_ptr< LeafNode< DataT > > fakeRoot = 
				boost::make_shared< LeafNode<DataT> >(BOX_CACHE.getShiftedBox(0));
		LEAVES_LIST->setNext(fakeRoot.get());
		fakeRoot->setPrevious(LEAVES_LIST.get());
		
		PushedLevel level = createLevel(fakeRoot.get());
		// then set the REAL root pointer!
		ROOT = level.newBranch;
		
		notifyChanges();
	}
	
	virtual ~Octree() {
		delete ROOT;
	}
	
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
	
	LeavesArrayPtr pushLevel(LeafConstPtr leaf) {
		
		// TODO acquire readLock
		
		PushedLevel newLevel = createLevel(leaf);
		
		// link new branch to remaining tree 
		/* TODO devo ricordarmi che queste modifiche non vanno effettuate finchè
		 * non viene invocato il metodo notifyChanges => il vecchio BranchNode
		 * deve rimanere non-cosciente del fatto che la vecchia foglia è stata
		 * sostituita da un nuovo BranchNode fino a quel momento
		 * Probabilmente sarà necessario prevedere una lista di "cambiamenti
		 * da fare"
		 */
		BranchPtr bnp = dynamic_cast<BranchPtr>(leaf->getFather());
		bnp->setChild(leaf->getChildIdx(), newLevel.newBranch);
		
		// TODO release readLock
		
		return newLevel.newLeaves;
	}
	
	void purgeLeaf(LeafPtr &leaf) {
		
		// TODO acquire readLock
		
		/* TODO probabilmente questa parte andrà spostata nella fase di notify
		 * e qui aggiungo semplicemente la foglia alla lista di foglie da
		 * eliminare (se una foglia viene messa 2 volte?? -> potrei prima fare
		 * la rimozione dal branch -che lancia eccezione se già stata fatta-
		 * e poi l'aggiustamento dei puntatori, salvandoli prima)
		 */
		// remove given leaf from LEAVES_LIST
		leaf->getPrevious()->setNext(leaf->getNext());
		if (leaf->getNext() != NULL) {
			leaf->getNext()->setPrevious(leaf->getPrevious());
		}
		
		// tells father to forget her
		BranchPtr bnp = dynamic_cast<BranchPtr>(leaf->getFather());
		bnp->deleteChild(leaf->getChildIdx());
		
		// TODO release readLock
	}
	
	
	/**
	 * 
	 * @param obb
	 * @param isom isometry transformation from this box (the MODEL) to given one
	 * @return
	 */
	LeavesDequePtr getIntersectingLeaves(const SimpleBox &obb,
			const Eigen::Isometry3d &isom, bool accurate) const {
		
		// TODO acquire ReadLock
		
		typedef boost::shared_ptr< std::deque< OctreeNodePtr > > NodeQueuePtr;
		
		NodeQueuePtr currLvlNodes = boost::make_shared< std::deque< OctreeNodePtr > >();
		for (int i = 0; i < BranchNode::N_CHILDREN; i++) {
			if (getRoot()->hasChild(i))
				currLvlNodes->push_back(getRoot()->getChild(i));
		}
		
		LeavesDequePtr intersectingLeaves = boost::make_shared< std::deque< LeafPtr > >();
		
		NodeQueuePtr nextLvlNodes = boost::make_shared< std::deque< OctreeNodePtr > >();
		
		do {
			OctreeNodePtr currNode = currLvlNodes->back();
			currLvlNodes->pop_back();
			
			ShiftedBox::ConstPtr currBox = currNode->getBox();
			
			// checks for intersection
			if (currBox->isIntersecting(obb, isom, accurate)) {
				
				/* currNode is intersecting given box so now we have to
				 * try to expand it or save it as an intersecting leaf
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
							if (bnp->hasChild(i))
								nextLvlNodes->push_back(bnp->getChild(i));
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
				nextLvlNodes = boost::make_shared< std::deque< OctreeNodePtr > >();
			}
			
		} while (!currLvlNodes->empty());
		
		// TODO release readLock
		
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
	DataDequePtr getStoredData(bool onlyIfChanged = false) const {
		// TODO acquire readLock
		
		DataDequePtr data = boost::make_shared< DataDeque >();
		
		if (onlyIfChanged) {
			// TODO reset 'changed' flag with an atomic compare-exchange operation
		}
		
		LeafPtr currLeaf = getFirstLeaf();
		do {
			data->push_back(currLeaf->getData());
		} while((currLeaf = currLeaf->getNext()) != NULL);
		
		// TODO release ReadLock
		return data;
	}
	
	
private:
	
	BranchPtr getRoot() const {
		return ROOT;
	}
	
	LeafPtr getFirstLeaf() const {
		return LEAVES_LIST->getNext();
	}
	
	struct PushedLevel {
		PushedLevel() : newLeaves(boost::make_shared< LeavesArray >()) { }
		
		BranchPtr newBranch;
		LeavesArrayPtr newLeaves;
	};
	
	PushedLevel createLevel(LeafConstPtr leaf) {
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
		LeafPtr prevNode = leaf->getPrevious();
		
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
			
			LeafPtr child = new LeafNode< DataT >(
					toRet.newBranch,
					i,
					boost::make_shared< ShiftedBox >(newBox),
					newDepth);
			
			// set previous & next pointers
			prevNode->setNext(child);
			child->setPrevious(prevNode);
			prevNode = child;
			
			toRet.newBranch->setChild(i, child);
			
			toRet.newLeaves->at(i) = child;
		}
		
		// ...adjust last child's next pointer and leaf->getNext prev pointer
		toRet.newLeaves->back()->setNext(leaf->getNext());
		if (leaf->getNext() != NULL)
			leaf->getNext()->setPrevious(toRet.newLeaves->back());
	
		return toRet;
	}
	
	friend std::ostream & operator<<(std::ostream &os, const Octree &tree) {
		os << "Octree(extent=[" << tree.EXTENT.transpose() << "])" << "->"
				<< *tree.ROOT;
		
		return os;
	}
	
	
};

#endif /* OCTREE_HPP_ */
