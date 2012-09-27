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

#include <Eigen/Geometry>

#include "SimpleBox.hpp"
#include "Corner.hpp"

Stock::Stock(const StockDescription &desc, u_int maxDepth, MesherType::Ptr mesher) :
	MAX_DEPTH(maxDepth),
	EXTENT(desc.getGeometry()->X, desc.getGeometry()->Y, desc.getGeometry()->Z),
	STOCK_MODEL_TRASLATION(EXTENT / 2.0),
	MODEL(EXTENT),
	MESHER(mesher)
{
	GeometryUtils::checkExtent(EXTENT);
	if(MAX_DEPTH <= 0)
		throw std::invalid_argument("max depth should be >0");
}

Stock::~Stock() { }

IntersectionResult Stock::intersect(const Cutter::ConstPtr &cutter,
		const Eigen::Isometry3f &rototras) {
	
	boost::chrono::thread_clock::time_point startTime = boost::chrono::thread_clock::now();
	
	Cutter::BoundingBoxInfo bboxInfo = cutter->getBoundingBox();
	
	SimpleBox cutterBox(bboxInfo.extents);
	
	/* in order to find bbox isometry i can think about converting a bbox point
	 * to a stock point: first the point should be converted bbox=>cutter basis
	 * with bboxInfo.rototraslation than another conversion is needed, that is,
	 * cutter=>stock basis using rototras. So the multiplication order should
	 * be the following (remember that the point is multiplied on the right):
	 * 
	 * Eigen::Isometry3f bboxIsom_stock = rototras * bboxInfo.rototraslation;
	 * 
	 * Now we have bboxIsom in respect to stock basis but we also need it
	 * in respect of MODEL basis: from stock to model there is only a
	 * translation (no rotation at all) and it is "additive" (in fact we have
	 * to perform a subtraction) to given rototras so...
	 * 
	 * Eigen::Isometry3f bboxIsom_model = STOCK_MODEL_TRASLATION.inverse() * bboxIsom_stock;
	 * 
	 * all summed up:
	 */
	
	Eigen::Isometry3f cutterIsom_model = STOCK_MODEL_TRASLATION.inverse() * rototras;
	
	Eigen::Isometry3f bboxIsom_model = cutterIsom_model * bboxInfo.rototraslation;
	
	OctreeType::LeavesDequePtr leaves = MODEL.getIntersectingLeaves(cutterBox, bboxIsom_model, true);
	
	/* the plan: analyze leaves one by one. In order to do so easily and
	 * trying to keep array size as small as possible we start from the
	 * end popping out a leaf on each round and inserting new leaves at the
	 * end ( O(1) time for deque: this should be true even for insertion / 
	 * removal from the beginning but i don't trust STL :P )
	 */
	IntersectionResult results;
	SimpleBox::CornerMatrix cachedMatrix;
	while (!leaves->empty()) {
		
		results.analyzed_leaves++;
		OctreeType::LeafPtr currLeaf = leaves->back(); leaves->pop_back();
		
		/* by now we only know that cutter bounding box is intersecting
		 * currLeaf (if the test has set to be faster but inaccurate it may
		 * even not touching) but we have no clue about the fact that real
		 * cutter is intersecting or not.
		 * In order to do so we have to try to push model depth as deep as
		 * allowed in order to check if some voxels are fully contained or, at
		 * least, some of their corners are inside/outside cutter blade
		 */
		
		/* TODO controllare in quanto per le foglie pushate questo calcolo è
		 * inutile: lo faccio qui all'inizio solo perchè voglio verificare se
		 * la foglia è completamente contenuta in modo da cancellarla subito
		 * e non farne il push
		 */
		VoxelInfo currInfo;
		buildInfos(currLeaf, cutter, cutterIsom_model, cachedMatrix, currInfo);
		
		if (currInfo.isContained()) {
			
			results.purged_leaves++;
			
			// update waste sum
			results.waste += calculateWaste(currLeaf, currInfo);
			
			// then delete currLeaf from the model
			MODEL.purgeLeaf(currLeaf);
			
		} else {
			
			// currLeaf is probably half inside and half outside
			
			if (canPushLevel(currLeaf)) {
				
				results.pushed_leaves++;
				
				// we can push another level so let's do it
				OctreeType::LeavesArrayPtr newLeaves = MODEL.pushLevel(currLeaf);
				
				/* now we have to check wether returned leaves intersect
				 * or not: we cannot simply check for each corner distance
				 * because all corners may be outside cutter but butter itself
				 * is intersecting the voxel
				 */
				OctreeType::LeavesArray::const_iterator newLeavesIt;
				for (newLeavesIt = newLeaves->begin(); newLeavesIt != newLeaves->end(); ++newLeavesIt) {
					
					const ShiftedBox::ConstPtr &sbp = (*newLeavesIt)->getBox();
					
					if (sbp->isIntersecting(cutterBox, bboxIsom_model, false)) {
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
				
				if (!currInfo.isIntersecting()) {
					/* we reach this point by two conditions: cutter is really
					 * intersecting but we don't have enough voxel resolution
					 * or we stuck upon a bounding-box approximation error;
					 * so let's increment proper counter and...
					 */
					
					results.intersection_approx_errors++;
					
					// ...i don't even update data (waste of time)
					continue;
				}
				
				results.waste += calculateWaste(currLeaf, currInfo);
				
				/* it may happen that after a bunch of partial erosions given
				 * leaf should be deleted (all corners have crossed cutter's
				 * limits at least once). Such a case is due to lack of
				 * resolution in model voxels.
				 */
				if (currInfo.isContained()) {
					results.lazy_purged_leaves++;
					
					MODEL.purgeLeaf(currLeaf);
				} else {
					results.updated_data_leaves++;
					
					MODEL.updateLeafData(currLeaf, boost::make_shared< VoxelInfo >(currInfo));
				}
			} // if (canPushLevel)
		} // if (isContained)
		
	} // foreach (leave in leaves)
	
	MODEL.notifyChanges();
	
	results.elapsedTime = boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::thread_clock::now() - startTime);
	
	return results;
}

void Stock::buildInfos(const OctreeType::LeafConstPtr &leaf, 
			const Cutter::ConstPtr &cutter, const Eigen::Isometry3f &isometry,
			SimpleBox::CornerMatrix &cachedMatrix, VoxelInfo &info) {
	
	/* we have to convert stockPoint in cutter basis: given isometry
	 * is for the cutter in respect of model basis, so we
	 * have to invert it to get model's roto-traslation in respect of
	 * cutter basis, that is, the isometry that converts model points in
	 * cutter points.
	 */
	leaf->getBox()->buildCornerMatrix(isometry.inverse(), cachedMatrix);
	
	info.reset();
	for (CornerIterator cit = CornerIterator::begin(); cit != CornerIterator::end(); ++cit) {
		const Eigen::Vector3f point = cachedMatrix.col(static_cast<u_char>(*cit));
		
		float distance = cutter->getDistance(Point3D(point));
		
		info.setInsideness(*cit, distance);
	}
	
}

float Stock::calculateWaste(const OctreeType::LeafConstPtr & leaf,
		VoxelInfo &newInfo) const {
	
	VoxInfoCPtrCRef leafData = leaf->getData();
	newInfo += *leafData;
	return getApproxWaste(*leaf->getBox()->getSimpleBox(),
			*leafData,
			newInfo);
	
}

float Stock::getApproxWaste(const SimpleBox &box, 
		const VoxelInfo &oldInfo, const VoxelInfo &updatedInfo) const {
	
	return intersectedVolume(box, updatedInfo) - intersectedVolume(box, oldInfo);
}

float Stock::intersectedVolume(const SimpleBox &box, const VoxelInfo &info) const {
	u_char nInsideCorners = info.getInsideCornersNumber();
	
	if (nInsideCorners == 0)
		return 0;
	
	if (nInsideCorners == Corner::N_CORNERS)
		return box.getVolume();
	
	// some corners are inside and some are outside
	
	// TODO maybe we should implement something more accurate
	return box.getVolume() * nInsideCorners / (float) Corner::N_CORNERS;
}

bool Stock::canPushLevel(const OctreeType::LeafPtr &leaf) const {
	return leaf->getDepth() < this->MAX_DEPTH;
}

Eigen::Vector3f Stock::getResolution() const {
	return (this->EXTENT / std::pow(2.0, (float)this->MAX_DEPTH));
}

std::ostream & operator<<(std::ostream &os, const Stock &stock) {
	os << "STOCK(extent=[" << stock.EXTENT.transpose()
			<< "];maxDepth=" << stock.MAX_DEPTH
			<< ";minBlockSize=[" << stock.getResolution().transpose()
			<< "])"
//			<< std::endl << stock.MODEL
			;
	
	return os;
}


Mesh::Ptr Stock::getMeshing() {
	
	DataView data = MODEL.getStoredData();
	
	return MESHER->buildMesh(data);
	
}

