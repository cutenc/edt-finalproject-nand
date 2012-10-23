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
#include <boost/make_shared.hpp>

#include <Eigen/Geometry>

#include "Corner.hpp"
#include "SimpleBox.hpp"
#include "Utilities.hpp"

class Voxel {
	
public:
	
	typedef boost::shared_ptr< Voxel > Ptr;
	typedef boost::shared_ptr< const Voxel > ConstPtr;
	
	typedef boost::array< Eigen::Vector3d, Corner::N_CORNERS > CornerArray;
	typedef boost::array< const Eigen::Vector3d, Corner::N_CORNERS > CornerConstArray;
	typedef boost::array< Corner::CornerType, Corner::N_CORNERS > CornerTypeArray;
	
private:
	
	static const CornerTypeArray DEFAULT_ORDER;
	SimpleBox::CornerMatrixConstPtr POINTS;
	
public:
	
	Voxel(const SimpleBox::CornerMatrixConstPtr &points, bool doCopy) :
			POINTS(points)
	{
		assert (points->cols() == Corner::N_CORNERS);
		
		if (doCopy) {
			POINTS = boost::allocate_shared< SimpleBox::CornerMatrix,
					SimpleBox::CornerMatrixAllocator >(
							SimpleBox::CornerMatrixAllocator(), *points
					);
		}
	}
	
	virtual ~Voxel() { }
	
	Eigen::Vector3d getCorner(Corner::CornerType c) const {
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
		os << *(voxel.POINTS);
		
		return os;
	}
	
	
private:
	
	Eigen::Vector3d getCorner(u_char i) const {
		return POINTS->col(i);
	}
	
	static CornerTypeArray buildDefaultOrder();
	
};


#endif /* VOXEL_HPP_ */
