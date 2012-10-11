/*
 * VoxelInfo.cpp
 *
 *  Created on: 22/set/2012
 *      Author: socket
 */

#include "VoxelInfo.hpp"

VoxelInfo::VoxelInfo() {
	assert(Corner::N_CORNERS == 8); // this class is full of shift op on chars
	
	insideCorners = 0x00;
}

VoxelInfo::VoxelInfo(double val) {
	assert(Corner::N_CORNERS == 8); // this class is full of shift op on chars
	
	insideCorners = 0x00;
	for (u_char i = 0; i < Corner::N_CORNERS; ++i) {
		setInsideness(i, val);
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

bool VoxelInfo::setInsideness(Corner::CornerType c, double insideness) {
	return setInsideness(static_cast<u_char>(c), insideness);
}

/**
 * You should use this method only in combination with the empty constructor
 * to initialize all corners of this object, but not for future
 * manipulation. Wrong usage of this method may result in current object
 * misbehaviour (in particular inside corner count may be incorrect,
 * affecting also methods like #isContained and #isIntersecting)
 * 
 * @param c
 * @param insideness
 * 
 * @see #updateInsideness
 */
bool VoxelInfo::setInsideness(Corner::CornerType c, double oldInsideness, double newInsideness) {
	return setInsideness(static_cast< u_char >(c), oldInsideness, newInsideness);
}

/**
 * Resets the number of inside corners to 0; you should call this
 * method only if you are going to call #setInsideness on each corner
 * after that.
 * 
 * @see #updateInsideness
 */
void VoxelInfo::reset() {
	this->insideCorners = 0x00;
}

bool VoxelInfo::updateInsideness(Corner::CornerType c, double insideness) {
	return updateInsideness(static_cast<u_char>(c), insideness);
}

VoxelInfo & VoxelInfo::operator+=(const VoxelInfo &v) {
	for (u_char i = 0; i < Corner::N_CORNERS; ++i) {
		this->updateInsideness(i, v.getInsideness(i));
	}
	
	return *this;
}

const VoxelInfo VoxelInfo::operator+(const VoxelInfo &v) {
	return VoxelInfo(*this) += v;
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
	return d >= 0;
}

double VoxelInfo::getInsideness(u_char i) const {
	return insideness[i];
}

bool VoxelInfo::setInsideness(u_char i, double newInsideness) {
	assert(i < 8); // MUST be less than 8 (strictly) due to shift op
	
	insideness[i] = newInsideness;
	
	if (isInside(newInsideness)) {
		insideCorners |= (0x01 << i);
		return true;
	}
	
	return false;
}

bool VoxelInfo::setInsideness(u_char i, double oldInsideness, double newInsideness) {
	assert(i < 8); // MUST be less than 8 (strictly) due to shift op
	insideness[i] = fmax(oldInsideness, newInsideness);
	
	if (isInside(insideness[i])) {
		insideCorners |= (0x01 << i);
		return true;
	}
	
	return false;
}

bool VoxelInfo::updateInsideness(u_char i, double newInsideness) {
	return updateInsideness(i, insideness[i], newInsideness);
}

bool VoxelInfo::updateInsideness(u_char i, double oldInsideness, double newInsideness) {
	insideness[i] = fmax(oldInsideness, newInsideness);
	
	if ((!isInside(oldInsideness)) && isInside(newInsideness)) {
		insideCorners |= (0x01 << i);
		return true;
	} /* there is no else clause because function used for update is a MAX
	 * so previously inside corners cannot fall outside
	*/
	
	return false;
}
