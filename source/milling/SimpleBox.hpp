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

#include <Eigen/Geometry>

#include "Corner.hpp"
#include "common/Point3D.hpp"
#include "common/Utilities.hpp"


class SimpleBox {
	
public:
	
	typedef boost::shared_ptr< SimpleBox > Ptr;
	typedef boost::shared_ptr< const SimpleBox > ConstPtr;
	
	
	/**
	 * Half dimensions
	 */
	const Eigen::Vector3d EXTENT;
	const double VOLUME;
	
public:
	SimpleBox(const Eigen::Vector3d &extent) :
			EXTENT(extent / 2.0),
			VOLUME(extent[0] * extent[1] * extent[2]) {
		
		GeometryUtils::checkExtent(EXTENT);
		if (VOLUME < std::numeric_limits<double>::epsilon()) {
			throw std::invalid_argument("Volume is too small or negative");
		}
	}
	
	virtual ~SimpleBox() { }
	
	Eigen::Vector3d getHalfExtent() const { return this->EXTENT; }
	Eigen::Vector3d getExtent() const { return this->EXTENT * 2.0; }
	
	friend std::ostream& operator<<(std::ostream &os, const SimpleBox &sb) {
		os << "BOX[" << sb.getExtent().transpose() << "]";
		return os;
	}
	
	/**
	 * 
	 * @param type
	 * @param traslation the traslation that have to be applied to returned
	 * points (for example if given box is placed somewhere in space different
	 * from the origin)
	 * @return
	 */
	Eigen::Vector3d getCorner(Corner::CornerType type,
			const Eigen::Vector3d &traslation = Eigen::Vector3d::Zero()) {
		return getSimpleCorner(type) + traslation;
	}
	
	Eigen::Vector3d getCorner(Corner::CornerType type,
			const Eigen::Matrix3d &rotation) {
		return getRotatedCorner(type, rotation);
	}
	
	/**
	 * 
	 * @param type
	 * @param traslation the tralsation that have to be applied to returned
	 * points: note that SimpleBox origin is ideally placed in its centroid.
	 * @param rotation the rotation that have to be applied to returned points
	 * (conversion from box basis - that is normals to its three dimensions - to 
	 * another one): note that Corner::CornerType is intended prior to rotation
	 * @return
	 */
	Eigen::Vector3d getCorner(Corner::CornerType type,
			const Eigen::Vector3d &traslation,
			const Eigen::Matrix3d &rotation) {
		return getRotatedCorner(type, rotation) + traslation;
	}
	
	double getVolume() const {
		return this->VOLUME;
	}
	
	
private:
	
	Eigen::Vector3d getSimpleCorner(Corner::CornerType type) const {
		
		Eigen::Vector3d corner = this->EXTENT;
		
		switch (type) {
			case Corner::BottomFrontLeft:
				corner[0] *= -1;
				corner[1] *= -1;
				corner[2] *= -1;
				break;
			case Corner::BottomFrontRight:
				corner[1] *= -1;
				corner[2] *= -1;
				break;
			case Corner::BottomRearLeft:
				corner[0] *= -1;
				corner[2] *= -1;
				break;
			case Corner::BottomRearRight:
				corner[2] *= -1;
				break;
			case Corner::UpperFrontLeft:
				corner[0] *= -1;
				corner[1] *= -1;
				break;
			case Corner::UpperFrontRight:
				corner[1] *= -1;
				break;
			case Corner::UpperRearLeft:
				corner[0] *= -1;
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
	
	Eigen::Vector3d getRotatedCorner(Corner::CornerType type, const Eigen::Matrix3d &rotation) const {
		return rotation * getSimpleCorner(type);
	}
	
};


#endif /* SIMPLEBOX_HPP_ */
