/*
 * CubeCache.cpp
 *
 *  Created on: 22/ott/2012
 *      Author: socket
 */

#include "CubeCache.hpp"

#include <Eigen/Geometry>

#include "visualizer/VisualizationUtils.hpp"

CubeCache::CubeCache() {
}

CubeCache::~CubeCache() {
}

osg::ref_ptr<osg::ShapeDrawable> CubeCache::getCube(const SimpleBox::ConstPtr& ptr) {
	
	CacheType::const_iterator elm = CACHE.find(ptr.get());
	
	if (elm == CACHE.end()) {
		// we have to build a new cube
		Eigen::Vector3d extents = ptr->getExtent();
		osg::ref_ptr< osg::ShapeDrawable > cube = VisualizationUtils::buildBox(
			extents[0],
			extents[1],
			extents[2]
		);
		CACHE[ptr.get()] = cube;
		
		return cube.get();
		
	} else {
		return elm->second.get();
	}
}
