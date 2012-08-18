/*
 * Point3D.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef POINT3D_HPP_
#define POINT3D_HPP_

#include <iostream>

class Point3D {
public:
	Point3D();
	Point3D(double x, double y, double z);
	
	virtual ~Point3D();
	
	friend std::ostream &operator<<(std::ostream &os, const Point3D &p) {
		os << "(" << p.X << "," << p.Y << "," << p.Z << ")";
		return os;
	}
	
	double X, Y, Z;
};

#endif /* POINT3D_HPP_ */
