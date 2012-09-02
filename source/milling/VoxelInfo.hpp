/*
 * MeshingInfo.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef MESHINGINFO_HPP_
#define MESHINGINFO_HPP_

#include <limits>
#include <algorithm>
#include <cassert>

#include "Corner.hpp"

class VoxelInfo {
	
	static const double MIN_INSIDENESS; 
	double insideness[CornerIterator::N_CORNERS];
	
public:
	VoxelInfo() {
		for (int i = 0; i < CornerIterator::N_CORNERS; ++i) {
			insideness[i] = MIN_INSIDENESS;
		}
	}
	
	virtual ~VoxelInfo() { }
	
	bool isIntersecting() const {
		for (u_char i = 0; i < CornerIterator::N_CORNERS; ++i) {
			if (isInside(i)) {
				return true;
			}
		}
		return false;
	}
	
	bool isContained() const {
		for (u_char i = 0; i < CornerIterator::N_CORNERS; ++i) {
			if (!isInside(i)) {
				return false;
			}
		}
		
		return true;
	}
	
	bool isInside(Corner c) const {
		return isInside(static_cast<u_char>(c));
	}
	
	u_char getInsideCornersNumber() const {
		u_char count = 0;
		for (u_char i = 0; i < CornerIterator::N_CORNERS; ++i) {
			if (isInside(i)) {
				count++;
			}
		}
		
		return count;
	}
	
	double getInsideness(Corner c) const {
		return getInsideness(static_cast<u_char>(c));
	}
	
	void updateInsideness(Corner c, double insideness) {
		return updateInsideness(static_cast<u_char>(c), insideness);
	}
	
	VoxelInfo & operator+=(const VoxelInfo &v) {
		for (int i = 0; i < CornerIterator::N_CORNERS; ++i) {
			this->updateInsideness(i, v.getInsideness(i));
		}
		
		return *this;
	}
	
	const VoxelInfo operator+(const VoxelInfo &v) {
		return VoxelInfo(*this) += v;
	}
	
private:
	
	bool isInside(u_char i) const {
		assert(CommonUtils::isBetween(i, 0, CornerIterator::N_CORNERS));
		
		return insideness[i] >= 0;
	}
	
	double getInsideness(u_char i) const {
		assert(CommonUtils::isBetween(i, 0, CornerIterator::N_CORNERS));
		
		return insideness[i];
	}
	
	void updateInsideness(u_char i, double newInsideness) {
		assert(CommonUtils::isBetween(i, 0, CornerIterator::N_CORNERS));
		
		insideness[i] = std::max< double >(insideness[i], newInsideness);
	}
	
};

const double VoxelInfo::MIN_INSIDENESS = (std::numeric_limits<double>::has_infinity) ? 
		-std::numeric_limits<double>::infinity() : -std::numeric_limits<double>::max();

#endif /* MESHINGINFO_HPP_ */
