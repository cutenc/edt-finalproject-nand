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
	
	Eigen::Vector3f point;
	
public:
	Point3D();
	Point3D(const Eigen::Vector3f &p);
	Point3D(float x, float y, float z);
	
	virtual ~Point3D();
	
	Eigen::Vector3f asVector() const {
		return point;
	}
	
	Eigen::Translation3f asTranslation() const {
		return Eigen::Translation3f(this->point);
	}
	
	float getX() const {
		return point[0];
	}
	
	float getY() const {
		return point[1];
	}
	
	float getZ() const {
		return point[2];
	}
	
	friend std::ostream &operator<<(std::ostream &os, const Point3D &p) {
		os << p.point.transpose();
		return os;
	}
};

#endif /* POINT3D_HPP_ */
