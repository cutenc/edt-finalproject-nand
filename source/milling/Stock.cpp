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
		
		static bool isLeafIntersecting(OctreeNode::ConstPtr node,
				const SimpleBox &obb,
				const Eigen::Isometry3d &isom,
				bool accurate,
				IntersectionResult &results) {
			
			const ShiftedBox::ConstPtr &currBox = node->getBox();
			if (!currBox->isIntersecting(obb, isom, accurate)) {
				results.intersection_approx_skips++;
				return false;
			}
			
			return true;
		}
		
//		static ShiftedBox::MinMaxVector getBoundingBoxMinMax(
//				const SimpleBox &obb,
//				const Eigen::Isometry3d &isom)
//				{
//			
//			SimpleBox::CornerMatrixConstPtr matrix = obb.getCorners(isom);
//			Eigen::Vector3d min = matrix->rowwise().minCoeff();
//			Eigen::Vector3d max = matrix->rowwise().maxCoeff();
//			return ShiftedBox::MinMaxVector(min, max);
//		}
//		
//		static bool isIntersecting(OctreeNode::ConstPtr node,
//				const ShiftedBox::MinMaxVector &boxMinMax) {
//			
//			const ShiftedBox::ConstPtr &currBox = node->getBox();
//			return currBox->isIntersecting(boxMinMax);
//		}
		
	};
	
	
	/* TODO ho creato la nuova funzione che controlla l'intersezione
	 * sfruttando min-max invece che la oriented box
	 */
//	ShiftedBox::MinMaxVector minMax = BranchChoser::getBoundingBoxMinMax(cutterInfo->bbox,
//			*(cutterInfo->bboxIsom_model));
//	OctreeType::BranchChoserFunction branchFunc =
//			boost::bind(&BranchChoser::isIntersecting, _1,
//					boost::cref(minMax)
//					);
	
	
	OctreeType::BranchChoserFunction branchFunc =
			boost::bind(&BranchChoser::isIntersecting, _1,
					boost::cref(cutterInfo->bbox),
					boost::cref(*(cutterInfo->bboxIsom_model)),
					true);
	
	
	
	IntersectionResult results;
	
	OctreeType::LeafProcesserFunction leafFunc = 
			boost::bind(&Stock::analyzeLeaf,
					this, _1,
					boost::cref(*cutterInfo),
					boost::ref(results));
	
	OctreeType::LeafChoserFunction leafChoserFunc = 
			boost::bind(&BranchChoser::isLeafIntersecting, _1,
				boost::cref(cutterInfo->bbox),
				boost::cref(*(cutterInfo->bboxIsom_model)),
				false,
				boost::ref(results));
	
	MODEL.processTree(branchFunc, leafFunc, leafChoserFunc);
	
	results.elapsedTime = boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::thread_clock::now() - startTime);
	
	return results;
}

Stock::OctreeType::OperationType Stock::analyzeLeaf(const OctreeType::LeafPtr &currLeaf, 
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
	VoxelInfo currInfo; WasteInfo waste;
	cutVoxel(currLeaf, cutterInfo, currInfo, waste);
	
	if (currInfo.isContained()) {
		
		if (waste.totInserted == Corner::N_CORNERS) {
			results.purged_leaves++;
			
		} else {
			/* it may happen that after a bunch of partial erosions given
			 * leaf should be deleted (all corners have crossed cutter's
			 * limits at least once). Such a case is due to lack of
			 * resolution in model voxels.
			 */
			results.lazy_purged_leaves++;
		}
		
		results.waste += calculateNewWaste(currLeaf, waste);
		
		// then delete currLeaf from the model
		return OctreeType::DELETE_LEAF;
		
	} else {
		
		// currLeaf is probably half inside and half outside
		
		if (canPushLevel(currLeaf)) {
			
			results.pushed_leaves++;
			
			// we can push another level so let's do it
			return OctreeType::PUSH_LEAF;
			
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
				return OctreeType::NO_OP;
			}
			
			results.updated_data_leaves++;
			results.waste += calculateNewWaste(currLeaf, waste);
			
			currLeaf->setData(
					boost::make_shared< VoxelInfo >(currInfo)
					);
			
			return OctreeType::NO_OP;
			
		} // if (canPushLevel)
	} // if (isContained)	
	
}


void Stock::cutVoxel(const OctreeType::LeafConstPtr &leaf,
		const CutterInfos &cutterInfo, VoxelInfo &info, WasteInfo &waste) const {
	
	/* we have to convert stockPoint in cutter basis: given isometry
	 * is for the cutter in respect of model basis, so we
	 * have to invert it to get model's roto-traslation in respect of
	 * cutter basis, that is, the isometry that converts model points in
	 * cutter points.
	 */
	SimpleBox::CornerMatrix cachedMatrix;
	const ShiftedBox::ConstPtr &box = leaf->getBox();
	box->buildCornerMatrix(
			cutterInfo.cutterIsom_model->inverse(), cachedMatrix
	);
	
	waste.reset(); info.reset();
	for (CornerIterator cit = CornerIterator::begin(); cit != CornerIterator::end(); ++cit) {
		const Eigen::Vector3d point = cachedMatrix.col(static_cast<u_char>(*cit));
		
		double distance = cutterInfo.cutter->getDistance(point);
		
		double oldDistance = leaf->getData()->getInsideness(*cit);
		bool isInside = info.setInsideness(*cit,
				oldDistance,
				distance);
		
		if (isInside) {
			if (!VoxelInfo::isInside(oldDistance)) {
				++waste.newInsideCorners;
			}
			if (VoxelInfo::isInside(distance)) {
				++waste.totInserted;
			}
		}
	}
}

double Stock::calculateNewWaste(const OctreeType::LeafPtr &currLeaf, const WasteInfo &info) {
	// Calculate the amount of new waste
	if (info.newInsideCorners == 0)
		return 0;
	
	const SimpleBox::ConstPtr &box = currLeaf->getBox()->getSimpleBox();
	if (info.newInsideCorners == Corner::N_CORNERS)
		return box->getVolume();
	
	// some corners are inside and some are outside
	
	// TODO maybe we should implement something more accurate
	return box->getVolume() * info.newInsideCorners / (double) Corner::N_CORNERS;
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

