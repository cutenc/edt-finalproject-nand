/*
 * Triangle.hpp
 *
 *  Created on: 17/ott/2012
 *      Author: alberto
 */

#ifndef TRIANGLE_HPP_
#define TRIANGLE_HPP_

#include <osg/Geometry>

class Triangle {
public:

	osg::ref_ptr<osg::Geometry> triangle;

	Triangle(osg::Vec3f a, osg::Vec3f b, osg::Vec3f c);
	virtual ~Triangle();

	Triangle& operator=(const Triangle& triangle) {
		return *this;
	}

};

#endif /* TRIANGLE_HPP_ */
