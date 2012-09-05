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

#include <Eigen/Geometry>

#include "Corner.hpp"
#include "common/Point3D.hpp"
#include "common/Utilities.hpp"


class SimpleBox {
	
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
	
	bool isIntersecting(const SimpleBox &otherBox,
			const Eigen::Vector3d &traslation,
			const Eigen::Matrix3d &rotation = Eigen::Matrix3d::Identity()) const {
		
		// thanks to http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php
		
		/*ALGORITHM: Use the separating axis test for all 15 potential
		separating axes. If a separating axis could not be found, the two
		boxes overlap. */
		
		double ra, rb, t;
		const Eigen::Vector3d &a = this->EXTENT, &b = otherBox.EXTENT;
		
		//A's basis vectors
		for(int i = 0; i < 3; i++ ) {
			ra = a[i];
			rb = b[0] * std::abs<double>((double)rotation(i, 0)) 
					+ b[1] * std::abs<double>((double)rotation(i, 1)) 
					+ b[2] * std::abs<double>((double)rotation(i, 2)) ;
			t = std::abs<double>((double) traslation[i]);
			if( t > ra + rb )
				return false;
		}

		//B's basis vectors
		for(int i = 0; i<3 ; i++ ) {
			ra = a[0] * std::abs<double>((double)rotation(0, i))
					+ a[1] * std::abs<double>((double)rotation(1, i))
					+ a[2] * std::abs<double>((double)rotation(2, i)); 
			rb = b[i];
			t = std::abs<double>((double)(
					traslation[0] * rotation(0, i)
					+ traslation[1] * rotation(1, i)
					+ traslation[2] * rotation(2, i)
			));
			if( t > ra + rb )
				return false;
		}

		//9 cross products

		//L = A0 x B0
		ra = a[1]*std::abs<double>((double)rotation(2, 0)) + a[2]*std::abs<double>((double)rotation(1, 0));
		rb = b[1]*std::abs<double>((double)rotation(0, 2)) + b[2]*std::abs<double>((double)rotation(0, 1));
		t = std::abs<double>((double)(traslation[2]*rotation(1, 0) - traslation[1]*rotation(2, 0)));
		if( t > ra + rb )
			return false;

		//L = A0 x B1
		ra = a[1]*std::abs<double>((double)rotation(2, 1)) + a[2]*std::abs<double>((double)rotation(1, 1));
		rb = b[0]*std::abs<double>((double)rotation(0, 2)) + b[2]*std::abs<double>((double)rotation(0, 0));
		t = std::abs<double>((double)(traslation[2]*rotation(1, 1) - traslation[1]*rotation(2, 1)));
		if( t > ra + rb )
			return false;

		//L = A0 x B2
		ra = a[1]*std::abs<double>((double)rotation(2, 2)) + a[2]*std::abs<double>((double)rotation(1, 2));
		rb = b[0]*std::abs<double>((double)rotation(0, 1)) + b[1]*std::abs<double>((double)rotation(0, 0));
		t = std::abs<double>((double)(traslation[2]*rotation(1, 2) - traslation[1]*rotation(2, 2)));
		if( t > ra + rb )
			return false;

		//L = A1 x B0
		ra = a[0]*std::abs<double>((double)rotation(2, 0)) + a[2]*std::abs<double>((double)rotation(0, 0));
		rb = b[1]*std::abs<double>((double)rotation(1, 2)) + b[2]*std::abs<double>((double)rotation(1, 1));
		t = std::abs<double>((double)(traslation[0]*rotation(2, 0) - traslation[2]*rotation(0, 0)));
		if( t > ra + rb )
			return false;

		//L = A1 x B1
		ra = a[0]*std::abs<double>((double)rotation(2, 1)) + a[2]*std::abs<double>((double)rotation(0, 1));
		rb = b[0]*std::abs<double>((double)rotation(1, 2)) + b[2]*std::abs<double>((double)rotation(1, 0));
		t = std::abs<double>((double)(traslation[0]*rotation(2, 1) - traslation[2]*rotation(0, 1)));
		if( t > ra + rb )
			return false;

		//L = A1 x B2
		ra = a[0]*std::abs<double>((double)rotation(2, 2)) + a[2]*std::abs<double>((double)rotation(0, 2));
		rb = b[0]*std::abs<double>((double)rotation(1, 1)) + b[1]*std::abs<double>((double)rotation(1, 0));
		t = std::abs<double>((double)(traslation[0]*rotation(2, 2) - traslation[2]*rotation(0, 2)));
		if( t > ra + rb )
			return false;

		//L = A2 x B0
		ra = a[0]*std::abs<double>((double)rotation(1, 0)) + a[1]*std::abs<double>((double)rotation(0, 0));
		rb = b[1]*std::abs<double>((double)rotation(2, 2)) + b[2]*std::abs<double>((double)rotation(2, 1));
		t = std::abs<double>((double)(traslation[1]*rotation(0, 0) - traslation[0]*rotation(1, 0)));
		if( t > ra + rb )
			return false;

		//L = A2 x B1
		ra = a[0]*std::abs<double>((double)rotation(1, 1)) + a[1]*std::abs<double>((double)rotation(0, 1));
		rb = b[0] *std::abs<double>((double)rotation(2, 2)) + b[2]*std::abs<double>((double)rotation(2, 0));
		t = std::abs<double>((double)(traslation[1]*rotation(0, 1) - traslation[0]*rotation(1, 1)));
		if( t > ra + rb )
			return false;

		//L = A2 x B2
		ra = a[0]*std::abs<double>((double)rotation(1, 2)) + a[1]*std::abs<double>((double)rotation(0, 2));
		rb = b[0]*std::abs<double>((double)rotation(2, 1)) + b[1]*std::abs<double>((double)rotation(2, 0));
		t = std::abs<double>((double)(traslation[1]*rotation(0, 2) - traslation[0]*rotation(1, 2)));
		if( t > ra + rb )
			return false;

		/*no separating axis found,
		the two boxes overlap */

		return true;

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
