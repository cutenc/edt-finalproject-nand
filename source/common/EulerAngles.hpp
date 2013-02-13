/**
 * @file EulerAngles.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef EULERANGLES_HPP_
#define EULERANGLES_HPP_

#include <iostream>
#include <cmath>


#include <Eigen/Geometry>

#include <osg/Geometry>

/**
 * @class EulerAngles
 *
 * Defines and manages 3D angles. Angles can be treated as Eigen or OSG angles
 */
class EulerAngles {
public:
	/**
	 * constructors
	 */
	EulerAngles();
	EulerAngles(double, double, double);
	
	virtual ~EulerAngles();
	
	/**
	 *
	 * @return angle as Eigen
	 */
	Eigen::Matrix3d asEigen() const {
		Eigen::Matrix3d mat;
		
		mat = Eigen::AngleAxisd(GAMMA, Eigen::Vector3d::UnitZ())
				* Eigen::AngleAxisd(BETA, Eigen::Vector3d::UnitY())
				* Eigen::AngleAxisd(ALPHA, Eigen::Vector3d::UnitX());
		
		return mat;
	}
	
	/**
	 *
	 * @return angle as OSG
	 */
	osg::Quat asOSG() const {
		return osg::Quat(ALPHA, osg::X_AXIS, BETA, osg::Y_AXIS, GAMMA, osg::Z_AXIS);
	}
	
	/**
	 * redefines the << operator for printing the angle
	 * @param os the output stream
	 * @param ea the Euler angle
	 * @return
	 */
	friend std::ostream &operator<<(std::ostream &os, const EulerAngles &ea) {
		os << ea.ALPHA << " " << ea.BETA << " " << ea.GAMMA;
		return os;
	}

	/**
	 * the three angles
	 */
	double ALPHA, BETA, GAMMA;
};

#endif /* EULERANGLES_HPP_ */
