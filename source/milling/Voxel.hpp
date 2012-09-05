/*
 * Voxel.hpp
 *
 *  Created on: 02/set/2012
 *      Author: socket
 */

#ifndef VOXEL_HPP_
#define VOXEL_HPP_

#include <vector>
#include <cassert>
#include <stdexcept>

#include <Eigen/Geometry>

#include "Corner.hpp"
#include "common/Utilities.hpp"

class Voxel {
	
	Eigen::Vector3d POINTS[Corner::N_CORNERS];
	
public:
	
	Voxel(const std::vector< Eigen::Vector3d > &points) {
		if (points.size() != Corner::N_CORNERS)
			throw std::invalid_argument("wrong number of points");
		
		int i = 0;
		std::vector< Eigen::Vector3d >::const_iterator pIt = points.begin();
		for (; pIt != points.end(); ++pIt) {
			POINTS[i++] = *pIt;
		}
	}
	
	virtual ~Voxel() { }
	
	Eigen::Vector3d getCorner(Corner::CornerType c) {
		return getCorner(static_cast<u_char>(c));
	}
	
private:
	
	Eigen::Vector3d getCorner(u_char i) {
		assert(CommonUtils::isBetween(i, 0, Corner::N_CORNERS));
		
		return POINTS[i];
	}
	
};


#endif /* VOXEL_HPP_ */
