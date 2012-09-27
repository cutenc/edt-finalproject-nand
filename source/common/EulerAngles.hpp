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
	EulerAngles(float, float, float);
	
	virtual ~EulerAngles();
	
	Eigen::Matrix3f asEigen() const {
		Eigen::Matrix3f mat;
		
		mat = Eigen::AngleAxisf(GAMMA, Eigen::Vector3f::UnitZ())
				* Eigen::AngleAxisf(BETA, Eigen::Vector3f::UnitY())
				* Eigen::AngleAxisf(ALPHA, Eigen::Vector3f::UnitX());
		
//		mat = Eigen::AngleAxisd(ALPHA, Eigen::Vector3d::UnitX())
//				* Eigen::AngleAxisd(BETA, Eigen::Vector3d::UnitY())
//				* Eigen::AngleAxisd(GAMMA, Eigen::Vector3d::UnitZ());
		
		return mat;
	}
	
	friend std::ostream &operator<<(std::ostream &os, const EulerAngles &ea) {
		os << ea.ALPHA << " " << ea.BETA << " " << ea.GAMMA;
		return os;
	}
	
	float ALPHA, BETA, GAMMA;
};

#endif /* EULERANGLES_HPP_ */
