/*
 * VoxelInfo.cpp
 *
 *  Created on: 22/set/2012
 *      Author: socket
 */

#include "VoxelInfo.hpp"

VoxelInfo::VoxelInfo(double val) {
	assert(Corner::N_CORNERS == 8); // this class is full of shift op on chars
	
	for (u_char i = 0; i < Corner::N_CORNERS; ++i) {
		insideness[i] = val;
	}
	
	if (isInside(val)) {
		insideCorners = 0xff;
	} else {
		insideCorners = 0x00;
	}
}

VoxelInfo::~VoxelInfo() { }

bool VoxelInfo::isIntersecting() const {
	return insideCorners > 0;
}

bool VoxelInfo::isContained() const {
	return insideCorners == 0xff;
}

bool VoxelInfo::isCornerCut(Corner::CornerType c) const {
	u_char i = static_cast<u_char>(c);
	
	assert(i < 8); // MUST be 8: see shift operation
	
	return insideCorners & (0x01 << i);
}

double VoxelInfo::getInsideness(Corner::CornerType c) const {
	return getInsideness(static_cast<u_char>(c));
}

/**
 * 
 * @param c
 * @param insideness
 * @return \c true if the corner become inside after the update, false if it
 * is still outside or it were already inside
 */
bool VoxelInfo::updateInsideness(Corner::CornerType c, double newInsideness) {
	
	if (isCornerCut(c)) {
		// already inside
		return false;
	}
	
	if (isInside(newInsideness)) {
		int i = static_cast< int >(c);
		
		assert(i >= 0 && i < 8);
		
		insideness[i] = newInsideness;
		insideCorners |= (0x01 << i);
		return true;
	}
	
	// still outside
	return false;
}

std::ostream & operator<<(std::ostream &os, const VoxelInfo &vinfo) {
	os << "[";
	for (u_char i = 0; i < Corner::N_CORNERS; ++i) {
		os << vinfo.isInside(i); 
	}
	os << "]";
	
	return os;
}

double VoxelInfo::DEFAULT_INSIDENESS() {
	static const double DEF_INSIDENESS = -CommonUtils::INFINITE();
	return DEF_INSIDENESS;
}

bool VoxelInfo::isInside(double d) {
	// -0,0 because -0.0 < 0.0
	return d >= -0.0;
}

osg::ref_ptr<osg::Geode> VoxelInfo::getGeode() const {
	return geode;
}

void VoxelInfo::setGeode(osg::ref_ptr<osg::Geode> geode) {
	this->geode = geode;
}

double VoxelInfo::getInsideness(u_char i) const {
	return insideness[i];
}

//bool VoxelInfo::updateInsideness(u_char i, double oldInsideness, double newInsideness) {
//	insideness[i] = fmax(oldInsideness, newInsideness);
//	
//	if ((!isInside(oldInsideness)) && isInside(newInsideness)) {
//		insideCorners |= (0x01 << i);
//		return true;
//	} /* there is no else clause because function used for update is a MAX
//	 * so previously inside corners cannot fall outside
//	*/
//	
//	return false;
//}
