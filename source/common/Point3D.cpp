/*
 * Point3D.cpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#include "Point3D.hpp"

Point3D::Point3D() : point(0, 0, 0) { }

Point3D::Point3D(Eigen::Vector3d p) : point(p) { };

Point3D::Point3D(double x, double y, double z) : point(x, y, z) { }

Point3D::~Point3D() {
}

