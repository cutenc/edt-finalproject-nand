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
	
	Eigen::Matrix3d asEigen() const {
		Eigen::Matrix3d mat;
		
		mat = Eigen::AngleAxisd(GAMMA, Eigen::Vector3d::UnitZ())
				* Eigen::AngleAxisd(BETA, Eigen::Vector3d::UnitY())
				* Eigen::AngleAxisd(ALPHA, Eigen::Vector3d::UnitX());
		
//		mat = Eigen::AngleAxisd(ALPHA, Eigen::Vector3d::UnitX())
//				* Eigen::AngleAxisd(BETA, Eigen::Vector3d::UnitY())
//				* Eigen::AngleAxisd(GAMMA, Eigen::Vector3d::UnitZ());
		
		return mat;
	}
	
	friend std::ostream &operator<<(std::ostream &os, const EulerAngles &ea) {
		os << ea.ALPHA << " " << ea.BETA << " " << ea.GAMMA;
		return os;
	}
	
	double ALPHA, BETA, GAMMA;
};

#endif /* EULERANGLES_HPP_ */
