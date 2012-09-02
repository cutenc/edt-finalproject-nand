/*
 * Point3D.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef POINT3D_HPP_
#define POINT3D_HPP_

#include <iostream>

#include <Eigen/Geometry>

class Point3D {
	
	Eigen::Vector3d point;
	
public:
	Point3D();
	Point3D(Eigen::Vector3d p);
	Point3D(double x, double y, double z);
	
	virtual ~Point3D();
	
	Eigen::Vector3d asEigen() {
		return point;
	}
	
	friend std::ostream &operator<<(std::ostream &os, const Point3D &p) {
		os << "(" << p << ")";
		return os;
	}
};

#endif /* POINT3D_HPP_ */
