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
	Point3D(const Eigen::Vector3d &p);
	Point3D(double x, double y, double z);
	
	virtual ~Point3D();
	
	Eigen::Vector3d asVector() const {
		return point;
	}
	
	Eigen::Translation3d asTranslation() const {
		return Eigen::Translation3d(this->point);
	}
	
	double getX() const {
		return point[0];
	}
	
	double getY() const {
		return point[1];
	}
	
	double getZ() const {
		return point[2];
	}

	void setX(double x) {
		point[0] = x;
	}

	void setY(double y) {
		point[1] = y;
	}

	void setZ (double z) {
		point[2] = z;
	}
	
	friend std::ostream &operator<<(std::ostream &os, const Point3D &p) {
		os << p.point.transpose();
		return os;
	}
};

#endif /* POINT3D_HPP_ */
