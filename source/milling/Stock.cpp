/*
 * Stock.cpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#include "Stock.hpp"

#include <vector>
#include <deque>
#include <stdexcept>
#include <cmath>

#include <boost/chrono.hpp>
#include <boost/container/slist.hpp>

#include <Eigen/Geometry>

#include "SimpleBox.hpp"
#include "Corner.hpp"

Stock::Stock(const StockDescription &desc, u_int maxDepth) :
	MAX_DEPTH(maxDepth),
	EXTENT(desc.getGeometry()->X, desc.getGeometry()->Y, desc.getGeometry()->Z),
	STOCK_MODEL_TRASLATION(EXTENT / -2.0),
	MODEL(EXTENT) {
	
	GeometryUtils::checkExtent(EXTENT);
	if(MAX_DEPTH <= 0)
		throw std::invalid_argument("max depth should be >0");
}

Stock::~Stock() { }

IntersectionResult Stock::intersect(Cutter::CutterPtr cutter, const Eigen::Vector3d &stockCutterTraslation, const Eigen::Matrix3d &rotation) {
	
	boost::chrono::thread_clock::time_point startTime = boost::chrono::thread_clock::now();
	
	Cutter::BoundingBoxInfo bboxInfo = cutter->getBoundingBox();
	
	SimpleBox cutterBox = bboxInfo.boundingBox;
	
	Eigen::Vector3d originTraslationRot = rotation * bboxInfo.originTraslation.asEigen();
	Eigen::Vector3d stockCutterBBoxTraslation = stockCutterTraslation + originTraslationRot;
	Eigen::Vector3d modelCutterBBoxTraslation = stockCutterBBoxTraslation + STOCK_MODEL_TRASLATION;
	Eigen::Vector3d modelCutterTrasl = stockCutterTraslation + STOCK_MODEL_TRASLATION;
	
	_Octree::LeavesDequePtr leaves = MODEL.getIntersectingLeaves(cutterBox, 
			modelCutterBBoxTraslation, rotation);
	
	/* the plan: analyze leaves one by one. In order to do so easily and
	 * trying to keep array size as small as possible we start from the
	 * end popping out a leaf on each round and inserting new leaves at the
	 * end
	 */
	
	IntersectionResult results;
	while (!leaves->empty()) {
		
		results.analyzed_leaves++;
		_Octree::LeafPtr currLeaf = leaves->back(); leaves->pop_back();
		
		VoxelInfo currInfo = buildInfos(currLeaf, cutter, modelCutterTrasl, rotation);
		
		if (!currInfo.isIntersecting()) {
			results.intersection_approx_errors++;
			
			// i don't even update data (waste of time)
			continue;
		}
		
		if (currInfo.isContained()) {
			
			results.purged_leaves++;
			
			// update waste sum
			results.waste += updateWaste(currLeaf, currInfo);
			
			// then delete currLeaf from the model
			MODEL.purgeLeaf(currLeaf);
			
		} else {
			
			// currLeaf is half inside and half outside
			
			if (canPushLevel(currLeaf)) {
				
				results.pushed_leaves++;
				
				// we can push another level so let's do it
				_Octree::LeavesDequePtr newLeaves = MODEL.pushLevel(currLeaf);
				
				/* now we have to check wether returned leaves intersect
				 * or not using approximated test (that should be
				 * faster than checking for every corner's distance)
				 */
				_Octree::LeavesDeque::const_iterator newLeavesIt;
				for (newLeavesIt = newLeaves->begin(); newLeavesIt != newLeaves->end(); ++newLeavesIt) {
					
					_Octree::SimpleBoxPtr sbp = MODEL.getSimpleBox(*newLeavesIt);
					Eigen::Vector3d leafCutterTrasl = modelCutterBBoxTraslation - (*newLeavesIt)->getTraslation();
					
					if (sbp->isIntersecting(cutterBox, leafCutterTrasl, rotation)) {
						/* let's add this new leaf to leaves array for 
						 * further investigation
						 */
						leaves->push_back(*newLeavesIt);
						
					} else {
						results.intersection_approx_skips++;
					}
				}
				
			} else {
				
				/* leaf is intersecting but i cannot push more levels
				 * so let's update saved data incrementing waste count
				 */
				results.waste += updateWaste(currLeaf, currInfo);
				
				/* it may happen that after a bunch of partial erosions given
				 * leaf should be deleted (all corners have crossed cutter's
				 * limits at least once)
				 */
				if (currLeaf->getDirtyData()->isContained()) {
					results.lazy_purged_leaves++;
					
					MODEL.purgeLeaf(currLeaf);
				}
			} // if (canPushLevel)
		} // if (isContained)
		
	} // foreach (leave in leaves)
	
	MODEL.notifyChanges();
	
	results.elapsedTime = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::thread_clock::now() - startTime);
	
	return results;
}

/**
 * 
 * @param leaf
 * @param cutter
 * @param traslation that is model-cutter traslation because internally
 * will use points in model basis that should be converted in cutter basis
 * @param rotation
 * @return
 */
VoxelInfo Stock::buildInfos(_Octree::LeafConstPtr leaf, 
		const Cutter::CutterPtr &cutter,
		const Eigen::Vector3d &traslation, const Eigen::Matrix3d &rotation) {
	
	VoxelInfo info;
	
	Voxel v = MODEL.getVoxel(leaf);
	for (CornerIterator cit = CornerIterator::begin(); cit != CornerIterator::end(); ++cit) {
		Eigen::Vector3d stockPoint = v.getCorner(*cit);
		
		/* now we have to convert stockPoint in cutter basis: given rotation
		 * and traslation are for the cutter in respect of model basis, so we
		 * have to invert them to get model's roto-traslation in respect of
		 * cutter basis.
		 * NB: due to the fact that rotation is an orthonormal base
		 * rot^(-1) = rot^T
		 * that is the inverse equals the transposed
		 */
		
		Eigen::Vector3d cutterPoint = rotation.transpose() * (stockPoint - traslation);
		
		double distance =  cutter->getDistance(Point3D(cutterPoint));
		
		info.updateInsideness(*cit, distance);
	}
	
	return info;
}

double Stock::updateWaste(_Octree::LeafPtr leaf, const VoxelInfo &newInfo) {
	
	// save old informations for incremental waste calculation
	VoxelInfo oldInfo = *(leaf->getData());
	
	// update saved informations with new ones
	(*leaf->getDirtyData()) += newInfo;
	
	// return an approximation of new waste
	_Octree::SimpleBoxPtr sbp = MODEL.getSimpleBox(leaf);
	return getApproxWaste(*sbp, oldInfo, *leaf->getDirtyData());
	
}

double Stock::getApproxWaste(const SimpleBox &box, 
		const VoxelInfo &oldInfo, const VoxelInfo &updatedInfo) const {
	
	return intersectedVolume(box, updatedInfo) - intersectedVolume(box, oldInfo);
}

double Stock::intersectedVolume(const SimpleBox &box, const VoxelInfo &info) const {
	u_char nInsideCorners = info.getInsideCornersNumber();
	
	if (nInsideCorners == 0)
		return 0;
	
	if (nInsideCorners == CornerIterator::N_CORNERS)
		return box.getVolume();
	
	// some corners are inside and some are outside
	
	// TODO maybe we should implement something more accurate
	return box.getVolume() * nInsideCorners / (double) CornerIterator::N_CORNERS;
}

bool Stock::canPushLevel(_Octree::LeafPtr leaf) const {
	return leaf->getDepth() < this->MAX_DEPTH;
}

Eigen::Vector3d Stock::getResolution() const {
	return (this->EXTENT / std::pow(2.0, (double)this->MAX_DEPTH));
}

std::ostream & operator<<(std::ostream &os, const Stock &stock) {
	os << "STOCK(minBlockSize=["
			<< stock.getResolution().transpose()
			<< "])"
			<< std::endl << stock.MODEL;
	
	return os;
}



