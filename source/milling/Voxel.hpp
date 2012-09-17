/*
 * Voxel.hpp
 *
 *  Created on: 02/set/2012
 *      Author: socket
 */

#ifndef VOXEL_HPP_
#define VOXEL_HPP_

#include <ostream>
#include <vector>
#include <cassert>
#include <stdexcept>

#include <boost/array.hpp>

#include <Eigen/Geometry>

#include "Corner.hpp"
#include "SimpleBox.hpp"
#include "common/Utilities.hpp"

class Voxel {
	
public:
	
	typedef boost::shared_ptr< Voxel > Ptr;
	typedef boost::shared_ptr< const Voxel > ConstPtr;
	
	typedef boost::array< Eigen::Vector3d, Corner::N_CORNERS > CornerArray;
	typedef boost::array< const Eigen::Vector3d, Corner::N_CORNERS > CornerConstArray;
	typedef boost::array< Corner::CornerType, Corner::N_CORNERS > CornerTypeArray;
	
private:
	
	static const CornerTypeArray DEFAULT_ORDER;
	CornerArray POINTS;
	
public:
	
	Voxel(const SimpleBox::CornerMatrixConstPtr &points) {
		if (points->cols() != Corner::N_CORNERS) {
			throw std::invalid_argument("Wrong number of corners");
		}
		
		for (int i = 0; i < Corner::N_CORNERS; ++i) {
			const Eigen::Vector3d &point = points->col(i);
			POINTS[i] = point;
		}
	}
	
	virtual ~Voxel() { }
	
	const Eigen::Vector3d & getCorner(Corner::CornerType c) const {
		return getCorner(static_cast<u_char>(c));
	}
	
	CornerArray getCorners(const CornerTypeArray &order = Voxel::DEFAULT_ORDER) const {
		assert(order.size() == Corner::N_CORNERS);
		
		CornerArray tmp;
		
		u_int i = 0;
		CornerTypeArray::const_iterator tit;
		for (tit = order.begin(); tit != order.end(); ++tit) {
			tmp[i++] = getCorner(*tit);
		}
		
		return tmp;
	}
	
	friend std::ostream & operator<<(std::ostream &os, const Voxel &voxel) {
		// I want to print all corner as a column matrix (it occupy less space)
		for (u_char i = 0; i < 3; ++i) {
			if (i != 0)
				os << std::endl;
			
			for (u_char j = 0; j < voxel.POINTS.size(); ++j) {
				if (j != 0)
					os << "\t";
				
				os << voxel.POINTS[j](i);
			}
			
		}
		
		return os;
	}
	
	
private:
	
	const Eigen::Vector3d &getCorner(u_char i) const {
		return POINTS[i];
	}
	
	static CornerTypeArray buildDefaultOrder();
	
};


#endif /* VOXEL_HPP_ */
