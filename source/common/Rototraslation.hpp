/**
 * @file Rototraslation.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef ROTOTRASLATION_HPP_
#define ROTOTRASLATION_HPP_

#include <iostream>

#include <Eigen/Geometry>

#include "Point3D.hpp"
#include "EulerAngles.hpp"

/**
 * @class Rototraslation
 *
 * Defines and manages a rototraslation in the 3D space
 */
class Rototraslation {
public:
	/**
	 * constructors
	 */
	Rototraslation();
	Rototraslation(const Point3D &, const EulerAngles &);
	/**
	 * destructor
	 */
	virtual ~Rototraslation();
	
	/**
	 * redefines the << operator for printing the rototraslation
	 * @param os the output stream
	 * @param r the rototraslation
	 * @return
	 */
	friend std::ostream &operator<<(std::ostream &os, const Rototraslation &r) {
		os << "ROTRAS(t=[" << r.TRASLATION << "]; r=[" << r.ROTATION << "])";
		return os;
	}
	
	/**
	 *
	 * @return the rototraslation as Eigen data
	 */
	Eigen::Isometry3d asEigen() const {
		
		/* I have to go this way in order do bypass an eigen error handling
		 * rotation matrix: doing something like
		 * Isometry3d = Translation3d * Matrix3d results in a wrong Isometry
		 * matrix (both rotation and translation)
		 */
		Eigen::Isometry3d tmpRot(ROTATION.asEigen());
		Eigen::Isometry3d rototras = TRASLATION.asTranslation() * tmpRot;
		
		return rototras;
	}

	/** the traslation */
	Point3D TRASLATION;
	/** the rotation */
	EulerAngles ROTATION;
};

#endif /* ROTOTRASLATION_HPP_ */
