/*
 * Rototraslation.hpp
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

class Rototraslation {
public:
	Rototraslation();
	Rototraslation(const Point3D &, const EulerAngles &);
	virtual ~Rototraslation();
	
	friend std::ostream &operator<<(std::ostream &os, const Rototraslation &r) {
		os << "ROTRAS(t=[" << r.TRASLATION << "]; r=[" << r.ROTATION << "])";
		return os;
	}
	
	Eigen::Isometry3f asEigen() const {
		
		/* I have to go this way in order do bypass an eigen error handling
		 * rotation matrix: doing something like
		 * Isometry3d = Translation3d * Matrix3d results in a wrong Isometry
		 * matrix (both rotation and translation)
		 */
		Eigen::Isometry3f tmpRot(ROTATION.asEigen());
		Eigen::Isometry3f rototras = TRASLATION.asTranslation() * tmpRot;
		
		return rototras;
	}

	Point3D TRASLATION;
	EulerAngles ROTATION;
};

#endif /* ROTOTRASLATION_HPP_ */
