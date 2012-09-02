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
	
	Octree<VoxelInfo>::LeavesDequePtr leaves = MODEL.getIntersectingLeaves(cutterBox, modelCutterTraslation, rotation);
	
	IntersectionResult results;
	
	Octree< VoxelInfo >::LeavesDeque::iterator leavesIt;
	for (leavesIt = leaves->begin(); leavesIt != leaves->end(); ++leavesIt) {
		
		Octree<VoxelInfo>::LeafPtr currLeaf = *leavesIt;
		
		if (/* leave's box does not intersect cutter model */) {
			continue;
		}
		
		if (/* leave's box is inside cutter model */) {
			
			/* ora per prima cosa check se il nodo è completamente interno al cutter
			 * e se si lo devo eliminare dall'albero, incrementando
			 * il conteggio del waste
			 * MODEL.purgeLeaf(currLeaf); <-- come ultima operazione in quanto
			 * elimina proprio la foglia (delete)
			 */
			// results.waste += MODEL.getVoxel(currLeaf)->getVolume();
			
		} else {
			
			/* il nodo è metà dentro e metà fuori (in ogni caso interseca)
			 */
			if (/* can push another level (i.e. depth < MAX_DEPTH */) {
				
				/* newLeaves = MODEL.pushLevel(currLeaf);
				 * leaves.insert(leaves.end(), newLeaves.begin(), newLeaves.end());
				 */
				
			} else {
				
				/* aggiorno i dati di leaves con la nuova porzione cancellata
				 * currLeave->getRawData() += currData;
				 */
				/* aggiornando i dati può succedere che la foglia corrente debba
				 * venir eliminata -> lo faccio
				 */
				
			}
			
		}
	}
	
	
	
	return results;
}

VoxelInfo Stock::buildInfos(Octree< VoxelInfo >::LeafConstPtr leaf, 
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
		 * that is the inverse equals the trasposed
		 */
		
		Eigen::Vector3d cutterPoint = rotation * (stockPoint - traslation);
		
		double distance =  cutter->getDistance(Point3D(cutterPoint));
		
		info.updateInsideness(*cit, distance);
	}
	
	return info;
}
	
double Stock::getApproxWaste(const SimpleBox &voxel, 
		const VoxelInfo &oldInfo, const VoxelInfo &updatedInfo) const {
	
	return intersectedVolume(voxel, updatedInfo) - intersectedVolume(voxel, oldInfo);
}

double Stock::intersectedVolume(const SimpleBox &voxel, const VoxelInfo &info) const {
	u_char nInsideCorners = info.getInsideCornersNumber();
	
	if (nInsideCorners == 0)
		return 0;
	
	if (nInsideCorners == CornerIterator::N_CORNERS)
		return voxel.getVolume();
	
	// some corners are inside and some are outside
	
	// TODO maybe we should implement something more accurate
	return voxel.getVolume() * nInsideCorners / (double) CornerIterator::N_CORNERS;
}




