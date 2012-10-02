/*
 * SimpleBox.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef SIMPLEBOX_HPP_
#define SIMPLEBOX_HPP_

#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <sstream>
#include <ostream>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/math/special_functions.hpp>

#include <Eigen/Geometry>

#include "Corner.hpp"
#include "common/Point3D.hpp"
#include "common/Utilities.hpp"


class SimpleBox {
	
public:
	
	typedef boost::shared_ptr< SimpleBox > Ptr;
	typedef boost::shared_ptr< const SimpleBox > ConstPtr;
	
	typedef Eigen::Matrix<double, 3, Corner::N_CORNERS> CornerMatrix;
	typedef boost::shared_ptr< CornerMatrix > CornerMatrixPtr;
	typedef boost::shared_ptr< const CornerMatrix > CornerMatrixConstPtr;
	
	typedef Eigen::aligned_allocator< CornerMatrix > CornerMatrixAllocator;
	
private:
	/**
	 * Half dimensions
	 */
	const Eigen::Vector3d EXTENT;
	const double VOLUME;
	const CornerMatrixConstPtr CORNERS;
	
public:
	SimpleBox(const Eigen::Vector3d &extent) :
			EXTENT(extent / 2.0),
			VOLUME(extent[0] * extent[1] * extent[2]),
			CORNERS(SimpleBox::buildCornerMatrix(EXTENT)) {
		
		GeometryUtils::checkExtent(EXTENT);
		if (VOLUME < std::numeric_limits<double>::epsilon()) {
			throw std::invalid_argument("Volume is too small or negative");
		}
	}
	
	virtual ~SimpleBox() { }
	
	const Eigen::Vector3d &getHalfExtent() const { return this->EXTENT; }
	Eigen::Vector3d getExtent() const { return this->EXTENT * 2.0; }
	
	friend std::ostream& operator<<(std::ostream &os, const SimpleBox &sb) {
		os << "BOX[" << sb.getExtent().transpose() << "]";
		return os;
	}
	
	Eigen::Vector3d getCorner(Corner::CornerType type, 
			const Eigen::Isometry3d &isometry = Eigen::Isometry3d::Identity()) {
		return isometry * getSimpleCorner(type);
	}
	
	CornerMatrixPtr getCorners(const Eigen::Isometry3d &isometry = Eigen::Isometry3d::Identity()) const {
		
		CornerMatrixPtr corners = boost::allocate_shared< CornerMatrix,
				CornerMatrixAllocator >( CornerMatrixAllocator() );
		
		buildCorners(isometry, *corners);
		
		return corners;
	}
	
	void buildCorners(const Eigen::Isometry3d &isometry, CornerMatrix &out) const {
		out.noalias() = isometry * (*this->CORNERS);
	}
	
	double getVolume() const {
		return this->VOLUME;
	}
	
	
protected:
	
	Eigen::Vector3d getSimpleCorner(Corner::CornerType type) const {
		u_char nCorner = static_cast<u_char>(type);
		
		return CORNERS->col(nCorner);
	}
	
	
private:
	
	static CornerMatrixPtr buildCornerMatrix(const Eigen::Vector3d &halfSizes) {
		
		CornerMatrixPtr corners = boost::allocate_shared< CornerMatrix, 
				CornerMatrixAllocator >(CornerMatrixAllocator());
		
		CornerIterator cit = CornerIterator::begin();
		for (; cit != CornerIterator::end(); ++cit) {
			Eigen::Vector3d corner = SimpleBox::getSimpleCorner(*cit, halfSizes);
			u_char nCorner = static_cast<u_char>(*cit);
			
			corners->col(nCorner) = corner;
		}
		
		return corners;
	}
	
	static Eigen::Vector3d getSimpleCorner(Corner::CornerType type, 
			const Eigen::Vector3d &halfSizes) {
			
		Eigen::Vector3d corner = halfSizes;
		
		switch (type) {
			case Corner::BottomFrontLeft:
				corner *= -1;
				break;
			case Corner::BottomFrontRight:
				corner[1] = boost::math::changesign(corner[1]);
				corner[2] = boost::math::changesign(corner[2]);
				break;
			case Corner::BottomRearLeft:
				corner[0] = boost::math::changesign(corner[0]);
				corner[2] = boost::math::changesign(corner[2]);
				break;
			case Corner::BottomRearRight:
				corner[2] = boost::math::changesign(corner[2]);
				break;
			case Corner::UpperFrontLeft:
				corner[0] = boost::math::changesign(corner[0]);
				corner[1] = boost::math::changesign(corner[1]);
				break;
			case Corner::UpperFrontRight:
				corner[1] = boost::math::changesign(corner[1]);
				break;
			case Corner::UpperRearLeft:
				corner[0] = boost::math::changesign(corner[0]);
				break;
			case Corner::UpperRearRight:
				// ok: all positive
				break;
				
			default:
				throw std::runtime_error("Given corner type does not exist");
				break;
		}
		
		return corner;
	}
	
};


#endif /* SIMPLEBOX_HPP_ */
