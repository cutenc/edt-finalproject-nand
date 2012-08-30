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
	
	Octree<MeshingInfo>::LeavesVectorPtr intLeaves = MODEL.getIntersectingLeaves(cutterBox, modelCutterTraslation, rotation);
	
	// TODO continuare
	
}



