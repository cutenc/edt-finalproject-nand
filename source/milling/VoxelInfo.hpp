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
	
	// updated by updateInsideness(u_char, double) function
	u_char insideCornerNumber;
	double insideness[Corner::N_CORNERS];
	
public:
	VoxelInfo() {
		for (int i = 0; i < Corner::N_CORNERS; ++i) {
			insideness[i] = -CommonUtils::INFINITE;
		}
		insideCornerNumber = 0;
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
	
private:
	
	inline
	bool isInside(u_char i) const {
		return insideness[i] >= 0;
	}
	
	double getInsideness(u_char i) const {
		return insideness[i];
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
