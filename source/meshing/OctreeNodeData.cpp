/*
 * OctreeNodeData.cpp
 *
 *  Created on: 24/ott/2012
 *      Author: socket
 */

#include "OctreeNodeData.hpp"

OctreeNodeData::OctreeNodeData(const osg::BoundingBoxd& bbox, unsigned char depth):
	bbox(bbox), depth(depth)
{ }

const osg::BoundingBoxd& OctreeNodeData::getCompetenceBox() const {
	return this->bbox;
}

bool OctreeNodeData::isMyCompetence(const osg::BoundingBoxd& box) {
	return ((this->bbox.contains(box._min) && this->bbox.contains(box._max)))
			|| 
			(this->bbox.intersects(box) && this->bbox.contains(box.center()));
}

OctreeNodeData::unsigned char OctreeNodeData::getDepth() const {
	return this->depth;
}

OctreeNodeData::~OctreeNodeData() {
}
