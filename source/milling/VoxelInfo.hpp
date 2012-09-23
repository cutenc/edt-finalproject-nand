/*
 * MeshingInfo.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef VOXELINFO_HPP_
#define VOXELINFO_HPP_

#include <ostream>
#include <limits>
#include <algorithm>
#include <cassert>

#include "Corner.hpp"
#include "common/Utilities.hpp"

class VoxelInfo {
	
private:
	// updated by updateInsideness(u_char, double) function
	u_char insideCornerNumber;
	double insideness[Corner::N_CORNERS];
	
public:
	
	/**
	 * This constructor leave object in a incosistent state and should be
	 * used only in conjunction with the #setInsideness method. It is
	 * provided just to increase performance avoiding useless initializations.
	 */
	VoxelInfo() {
		insideCornerNumber = 0;
	}
	
	VoxelInfo(double val) {
		insideCornerNumber = 0;
		for (u_char i = 0; i < Corner::N_CORNERS; ++i) {
			setInsideness(i, val);
		}
	}
	
	virtual ~VoxelInfo() { }
	
	inline
	bool isIntersecting() const {
		return insideCornerNumber > 0;
	}
	
	inline
	bool isContained() const {
		return insideCornerNumber == Corner::N_CORNERS;
	}
	
	inline
	bool isInside(Corner::CornerType c) const {
		return isInside(static_cast<u_char>(c));
	}
	
	u_char getInsideCornersNumber() const {
		return insideCornerNumber;
	}
	
	double getInsideness(Corner::CornerType c) const {
		return getInsideness(static_cast<u_char>(c));
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
	void setInsideness(Corner::CornerType c, double insideness) {
		return setInsideness(static_cast<u_char>(c), insideness);
	}
	
	void updateInsideness(Corner::CornerType c, double insideness) {
		return updateInsideness(static_cast<u_char>(c), insideness);
	}
	
	VoxelInfo & operator+=(const VoxelInfo &v) {
		for (int i = 0; i < Corner::N_CORNERS; ++i) {
			this->updateInsideness(i, v.getInsideness(i));
		}
		
		return *this;
	}
	
	const VoxelInfo operator+(const VoxelInfo &v) {
		return VoxelInfo(*this) += v;
	}
	
	friend std::ostream & operator<<(std::ostream &os, const VoxelInfo &vinfo) {
		os << "[";
		for (u_char i = 0; i < Corner::N_CORNERS; ++i) {
			os << vinfo.isInside(i); 
		}
		os << "]";
		
		return os;
	}
	
	inline
	static double DEFAULT_INSIDENESS() {
		static const double DEF_INSIDENESS = -CommonUtils::INFINITE();
		return DEF_INSIDENESS;
	}
	
private:
	
	inline
	bool isInside(u_char i) const {
		return insideness[i] >= 0;
	}
	
	double getInsideness(u_char i) const {
		return insideness[i];
	}
	
	void setInsideness(u_char i, double val) {
		insideness[i] = val;
		if (isInside(i)) {
			++insideCornerNumber;
		}
	}
	
	void updateInsideness(u_char i, double newInsideness) {
		bool prevInside = isInside(i);
		
		insideness[i] = std::max< double >(insideness[i], newInsideness);
		
		if (!prevInside && isInside(i)) {
			++insideCornerNumber;
		} /* there is no else clause because function used for update is a MAX
		 * so previously inside corners cannot fall outside
		*/ 
	}
	
};

#endif /* VOXELINFO_HPP_ */
