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
	
	Eigen::Isometry3d asEigen() const {
		Eigen::Isometry3d rototras(TRASLATION.asTranslation());
		rototras *= ROTATION.asEigen();
		
		return rototras;
	}

	Point3D TRASLATION;
	EulerAngles ROTATION;
};

#endif /* ROTOTRASLATION_HPP_ */
