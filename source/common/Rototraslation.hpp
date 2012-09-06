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
	
	Eigen::Affine3d asEigen() const {
		Eigen::Affine3d t(Eigen::Translation3d(TRASLATION.asEigen()));
		t *= (Eigen::AngleAxisd(0.5 * M_PI, Eigen::Vector3d::UnitZ()));
		t *= (Eigen::AngleAxisd(-0.5 * M_PI, Eigen::Vector3d::UnitY()));
		t *= (Eigen::AngleAxisd(0 * M_PI, Eigen::Vector3d::UnitX()));
		
		return t;
	}

	Point3D TRASLATION;
	EulerAngles ROTATION;
};

#endif /* ROTOTRASLATION_HPP_ */
