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

#include <boost/array.hpp>

#include <Eigen/Geometry>

#include "Corner.hpp"
#include "common/Utilities.hpp"

class Voxel {
	
public:
	
	typedef boost::array< Eigen::Vector3d, Corner::N_CORNERS > CornerArray;
	typedef boost::array< const Eigen::Vector3d, Corner::N_CORNERS > CornerConstArray;
	typedef boost::array< Corner::CornerType, Corner::N_CORNERS > CornerTypeArray;
	
private:
	
	static const CornerTypeArray DEFAULT_ORDER;
	CornerConstArray POINTS;
	
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
	
	const Eigen::Vector3d & getCorner(Corner::CornerType c) const {
		return getCorner(static_cast<u_char>(c));
	}
	
	CornerArray getCorners(const CornerTypeArray &order) const {
		assert(order.size() == Corner::N_CORNERS);
		
		CornerArray tmp;
		
		u_int i = 0;
		CornerTypeArray::const_iterator tit;
		for (tit = order.begin(); tit != order.end(); ++tit) {
			tmp[i++] = getCorner(*tit);
		}
		
		return tmp;
	}
	
	CornerArray getCorners() const {
		return getCorners(Voxel::DEFAULT_ORDER);
	}
	
private:
	
	const Eigen::Vector3d &getCorner(u_char i) const {
		return POINTS[i];
	}
	
	static CornerTypeArray buildDefaultOrder();
	
};


#endif /* VOXEL_HPP_ */
