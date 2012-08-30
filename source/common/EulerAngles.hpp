/*
 * EulerAngles.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef EULERANGLES_HPP_
#define EULERANGLES_HPP_

#include <iostream>
#include <cmath>

#include <Eigen/Geometry>
#include <boost/math/constants/constants.hpp>

class EulerAngles {
public:
	EulerAngles();
	EulerAngles(double, double, double);
	
	virtual ~EulerAngles();
	
	Eigen::Matrix3d asEigen() {
		Eigen::Matrix3d mat;
		double pi = boost::math::constants::pi<double>();
		
		mat = Eigen::AngleAxisd(ALPHA * pi, Eigen::Vector3d::UnitX())
				* Eigen::AngleAxisd(BETA * pi, Eigen::Vector3d::UnitY())
				* Eigen::AngleAxisd(GAMMA * pi, Eigen::Vector3d::UnitZ());
		
		return mat;
	}
	
	friend std::ostream &operator<<(std::ostream &os, const EulerAngles &ea) {
		os << "(" << ea.ALPHA << "," << ea.BETA << "," << ea.GAMMA << ")";
		return os;
	}
	
	double ALPHA, BETA, GAMMA;
};

#endif /* EULERANGLES_HPP_ */