/*
 * Point3D.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef POINT3D_HPP_
#define POINT3D_HPP_

class Point3D {
public:
	Point3D();
	Point3D(double x, double y, double z);
	Point3D(Point3D &p);
	
	virtual ~Point3D();
	
	const double X, Y, Z;
};

#endif /* POINT3D_HPP_ */
