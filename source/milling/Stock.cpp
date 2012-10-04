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

#include <boost/utility.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/assign/ptr_list_inserter.hpp>

#include <Eigen/Geometry>

#include "threading/Runnable.hpp"
#include "SimpleBox.hpp"
#include "Corner.hpp"


Stock::Stock(const StockDescription &desc, u_int maxDepth, u_int maxThreads, MesherType::Ptr mesher) :
	MAX_DEPTH(maxDepth),
	EXTENT(desc.getGeometry()->X, desc.getGeometry()->Y, desc.getGeometry()->Z),
	STOCK_MODEL_TRASLATION(EXTENT / 2.0),
	MAX_THREADS(maxThreads),
	MODEL(EXTENT, maxDepth),
	MESHER(mesher)
{
	GeometryUtils::checkExtent(EXTENT);
	if(MAX_DEPTH <= 0)
		throw std::invalid_argument("max depth should be >0");
	if (MAX_THREADS < 1)
		throw std::invalid_argument("max threads should be >= 1");
}

Stock::~Stock() { }

IntersectionResult Stock::intersect(const Cutter::ConstPtr &cutter,
		const Eigen::Isometry3d &rototras) {
	
	boost::chrono::thread_clock::time_point startTime = boost::chrono::thread_clock::now();
	
	Cutter::BoundingBoxInfo bboxInfo = cutter->getBoundingBox();
	
	/* in order to find bbox isometry i can think about converting a bbox point
	 * to a stock point: first the point should be converted bbox=>cutter basis
	 * with bboxInfo.rototraslation than another conversion is needed, that is,
	 * cutter=>stock basis using rototras. So the multiplication order should
	 * be the following (remember that the point is multiplied on the right):
	 * 
	 * Eigen::Isometry3d bboxIsom_stock = rototras * bboxInfo.rototraslation;
	 * 
	 * Now we have bboxIsom in respect to stock basis but we also need it
	 * in respect of MODEL basis: from stock to model there is only a
	 * translation (no rotation at all) and it is "additive" (in fact we have
	 * to perform a subtraction) to given rototras so...
	 * 
	 * Eigen::Isometry3d bboxIsom_model = STOCK_MODEL_TRASLATION.inverse() * bboxIsom_stock;
	 * 
	 * all summed up:
	 */
	
	Eigen::Isometry3d cutterIsom_model = STOCK_MODEL_TRASLATION.inverse() * rototras;
	
	Eigen::Isometry3d bboxIsom_model = cutterIsom_model * bboxInfo.rototraslation;
	
	CutterInfos::ConstPtr cutterInfo = boost::make_shared< const CutterInfos >(
			cutter, bboxInfo.extents, cutterIsom_model, bboxIsom_model);
	
	struct BranchChoser {
		static bool isIntersecting(OctreeNode::ConstPtr node,
				const SimpleBox &obb,
				const Eigen::Isometry3d &isom,
				bool accurate) {
			
			const ShiftedBox::ConstPtr &currBox = node->getBox();
			return currBox->isIntersecting(obb, isom, accurate);
		}
	};
	
	OctreeType::BranchChoserFunction branchFunc =
			boost::bind(&BranchChoser::isIntersecting, _1,
					boost::cref(cutterInfo->bbox),
					boost::cref(*(cutterInfo->bboxIsom_model)),
					true);
	
	IntersectionResult results;
	
	OctreeType::LeafProcesserFunction leafFunc = 
			boost::bind(&Stock::analyzeLeafRecursive,
					this, _1,
					boost::cref(*cutterInfo),
					boost::ref(results));
	
	MODEL.processTree(branchFunc, leafFunc, true);
	
	results.elapsedTime = boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::thread_clock::now() - startTime);
	
	return results;
}

void Stock::analyzeLeafRecursive(const OctreeType::LeafPtr &currLeaf, 
		const CutterInfos &cutterInfo, IntersectionResult &results) {
	
	results.analyzed_leaves++;
	
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
	buildInfos(currLeaf, cutterInfo.cutter, *(cutterInfo.cutterIsom_model), currInfo);
	
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
			OctreeType::LeavesArray::iterator newLeavesIt;
			for (newLeavesIt = newLeaves->begin(); newLeavesIt != newLeaves->end(); ++newLeavesIt) {
				
				const ShiftedBox::ConstPtr &sbp = (*newLeavesIt)->getBox();
				
				if (sbp->isIntersecting((cutterInfo.bbox), *(cutterInfo.bboxIsom_model), false)) {
					/* this leaf is intersecting, so we have to make a
					 * recursive call to analyze it.
					 */
					analyzeLeafRecursive(*newLeavesIt, cutterInfo, results);
					
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
				return;
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
	
}


void Stock::buildInfos(const OctreeType::LeafConstPtr &leaf, 
		const Cutter::ConstPtr &cutter, const Eigen::Isometry3d &isometry,
		VoxelInfo &info) {
	
	/* we have to convert stockPoint in cutter basis: given isometry
	 * is for the cutter in respect of model basis, so we
	 * have to invert it to get model's roto-traslation in respect of
	 * cutter basis, that is, the isometry that converts model points in
	 * cutter points.
	 */
	SimpleBox::CornerMatrix cachedMatrix;
	leaf->getBox()->buildCornerMatrix(isometry.inverse(), cachedMatrix);
	
	info.reset();
	for (CornerIterator cit = CornerIterator::begin(); cit != CornerIterator::end(); ++cit) {
		const Eigen::Vector3d point = cachedMatrix.col(static_cast<u_char>(*cit));
		
		double distance = cutter->getDistance(point);
		
		info.setInsideness(*cit, distance);
	}
	
}

double Stock::calculateWaste(const OctreeType::LeafConstPtr & leaf,
		VoxelInfo &newInfo) const {
	
	VoxInfoCPtrCRef leafData = leaf->getData();
	newInfo += *leafData;
	return getApproxWaste(*leaf->getBox()->getSimpleBox(),
			*leafData,
			newInfo);
	
}

double Stock::getApproxWaste(const SimpleBox &box, 
		const VoxelInfo &oldInfo, const VoxelInfo &updatedInfo) const {
	
	return intersectedVolume(box, updatedInfo) - intersectedVolume(box, oldInfo);
}

double Stock::intersectedVolume(const SimpleBox &box, const VoxelInfo &info) const {
	u_char nInsideCorners = info.getInsideCornersNumber();
	
	if (nInsideCorners == 0)
		return 0;
	
	if (nInsideCorners == Corner::N_CORNERS)
		return box.getVolume();
	
	// some corners are inside and some are outside
	
	// TODO maybe we should implement something more accurate
	return box.getVolume() * nInsideCorners / (double) Corner::N_CORNERS;
}

bool Stock::canPushLevel(const OctreeType::LeafPtr &leaf) const {
	return leaf->getDepth() < this->MAX_DEPTH;
}

Eigen::Vector3d Stock::getResolution() const {
	return (this->EXTENT / std::pow(2.0, (double)this->MAX_DEPTH));
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

