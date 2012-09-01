/*
 * Stock.cpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#include "Stock.hpp"

#include <vector>
#include <stdexcept>

#include <Eigen/Geometry>

#include "SimpleBox.hpp"

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

IntersectionResult Stock::intersect(CutterPtr cutter, Eigen::Vector3d traslation, Eigen::Matrix3d rotation) {
	
	Cutter::BoundingBoxInfo bboxInfo = cutter->getBoundingBox();
	
	SimpleBox cutterBox = bboxInfo.boundingBox;
	
	Eigen::Vector3d boxCutterTraslation = traslation + bboxInfo.originTraslation.asEigen();
	Eigen::Vector3d modelCutterTraslation = boxCutterTraslation + STOCK_MODEL_TRASLATION;
	
	Octree<MeshingInfo>::LeavesVectorPtr leaves = MODEL.getIntersectingLeaves(cutterBox, modelCutterTraslation, rotation);
	
	IntersectionResult results;
	
	if (/* leave's box is inside cutter */) {
		
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
	
	
	
	return results;
}



