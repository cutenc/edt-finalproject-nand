/*
 * VoxelInfo.cpp
 *
 *  Created on: 22/set/2012
 *      Author: socket
 */

#include "VoxelInfo.hpp"

VoxelInfo::VoxelInfo(double val) {
	assert(Corner::N_CORNERS == 8); // this class is full of shift op on chars
	
	if (isInside(val)) {
		insideCorners = 0xff;
	} else {
		insideCorners = 0x00;
	}
}

VoxelInfo::~VoxelInfo() {
	
}


std::ostream & operator<<(std::ostream &os, const VoxelInfo &vinfo) {
	os << "[";
	for (int i = 0; i < Corner::N_CORNERS; ++i) {
		os << vinfo.isInside(i); 
	}
	os << "]";
	
	return os;
}

const GraphicPointer& VoxelInfo::getGraphics() const {
	return this->graphics;
}

void VoxelInfo::setGraphics(const GraphicPointer& graphics) {
	this->graphics = graphics;
}

bool VoxelInfo::hasGraphics() const {
	return this->graphics.node.valid();
}

