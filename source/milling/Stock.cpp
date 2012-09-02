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

#include <Eigen/Geometry>

#include "SimpleBox.hpp"
#include "Corner.hpp"

Stock::Stock(const StockDescription &desc, u_int maxDepth) :
	MAX_DEPTH(maxDepth),
	EXTENT(desc.getGeometry()->X, desc.getGeometry()->Y, desc.getGeometry()->Z),
	STOCK_MODEL_TRASLATION(EXTENT / -2.0),
	MODEL(EXTENT) {
	
	GeometryUtils::checkExtent(EXTENT);
	if(MAX_DEPTH <= 1)
		throw std::invalid_argument("max depth should be >1");
}

Stock::~Stock() { }

IntersectionResult Stock::intersect(CutterPtr cutter, const Eigen::Vector3d &traslation, const Eigen::Matrix3d &rotation) {
	
	Cutter::BoundingBoxInfo bboxInfo = cutter->getBoundingBox();
	
	SimpleBox cutterBox = bboxInfo.boundingBox;
	
	Eigen::Vector3d boxCutterTraslation = traslation + bboxInfo.originTraslation.asEigen();
	Eigen::Vector3d modelCutterTraslation = boxCutterTraslation + STOCK_MODEL_TRASLATION;
	
	_Octree::LeavesDequePtr leaves = MODEL.getIntersectingLeaves(cutterBox, modelCutterTraslation, rotation);
	
	IntersectionResult results;
	
	_Octree::LeavesDeque::iterator leavesIt;
	for (leavesIt = leaves->begin(); leavesIt != leaves->end(); ++leavesIt) {
		
		_Octree::LeafPtr currLeaf = *leavesIt;
		
		VoxelInfo currInfo = buildInfos(currLeaf, cutter, traslation, rotation);
		
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
					Eigen::Vector3d leafCutterTrasl = modelCutterTraslation - (*newLeavesIt)->getTraslation();
					
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
					results.lazy_leaf_purging++;
					
					MODEL.purgeLeaf(currLeaf);
				}
			} // if (canPushLevel)
		} // if (isContained)
		
	} // foreach (leave in leaves)
	
	results.analyzed_leaves = leaves->size();
	
	MODEL.notifyChanges();
	
	return results;
}

VoxelInfo Stock::buildInfos(_Octree::LeafConstPtr leaf, 
		const CutterPtr &cutter,
		const Eigen::Vector3d &traslation, const Eigen::Matrix3d &rotation) {
	
	VoxelInfo info;
	
	Voxel v = MODEL.getVoxel(leaf);
	for (CornerIterator cit = CornerIterator::begin(); cit != CornerIterator::end(); ++cit) {
		Eigen::Vector3d stockPoint = v.getCorner(*cit);
		
		/* now we have to convert stockPoint in cutter basis: given rotation
		 * and traslation are for the cutter in respect of stock basis, so we
		 * have to invert them to get stock's roto-traslation in respect of
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





