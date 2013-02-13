/**
 * @file Point3D.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef POINT3D_HPP_
#define POINT3D_HPP_

#include <iostream>

#include <Eigen/Geometry>

#include <osg/Geometry>

/**
 * @class Point3D
 *
 * Defines a 3D Point, either as Eigen point or triplet of doubles
 */
class Point3D {
	
	Eigen::Vector3d point;
	
public:
	/**
	 * constructors
	 */
	Point3D();
	Point3D(const Eigen::Vector3d &p);
	Point3D(double x, double y, double z);
	
	virtual ~Point3D();
	
	/**
	 *
	 * @return the point as Eigen Vector3d
	 */
	Eigen::Vector3d asVector() const {
		return point;
	}
	
	/**
	 *
	 * @return the point as an Eigen traslation
	 */
	Eigen::Translation3d asTranslation() const {
		return Eigen::Translation3d(this->point);
	}
	
	/**
	 *
	 * @return the point as a OSG vector of the three coordinates
	 */
	osg::Vec3d asOSG() const {
		return osg::Vec3d(point[0], point[1], point[2]);
	}
	
	/**
	 *
	 * @return the x coord
	 */
	double getX() const {
		return point[0];
	}
	
	/**
	 *
	 * @return the y coord
	 */
	double getY() const {
		return point[1];
	}
	
	/**
	 *
	 * @return the z coord
	 */
	double getZ() const {
		return point[2];
	}
	
	/**
	 * redefines the << operator for printing the point
	 * @param os the output stream
	 * @param p the point
	 * @return
	 */
	friend std::ostream &operator<<(std::ostream &os, const Point3D &p) {
		os << p.point.transpose();
		return os;
	}
};

#endif /* POINT3D_HPP_ */
