/*
 * VisualizationUtils.hpp
 *
 *  Created on: 20/ott/2012
 *      Author: socket
 */

#ifndef VISUALIZATIONUTILS_HPP_
#define VISUALIZATIONUTILS_HPP_

#include <osg/ShapeDrawable>
#include <osg/Geometry>

class VisualizationUtils {
public:
	
	/**
	 * Return a reference to a 3D axis system placed in the origin and not rotated.
	 * Note that returned class must not be modified because it is shared among
	 * everyone who calls this method.
	 * @return
	 */
	static osg::ref_ptr< osg::Geometry > getAxis();
	
	static osg::ref_ptr< osg::ShapeDrawable > buildCylinder(const osg::Vec3 &origin, float radius, float height);
	static osg::ref_ptr< osg::ShapeDrawable > buildCylinder(float radius, float height);
	
	static osg::ref_ptr< osg::ShapeDrawable > buildSphere(const osg::Vec3 &origin, float radius);
	static osg::ref_ptr< osg::ShapeDrawable > buildSphere(float radius);
	
	static osg::ref_ptr< osg::ShapeDrawable > buildBox(const osg::Vec3 &origin, float x, float y, float z);
	static osg::ref_ptr< osg::ShapeDrawable > buildBox(float x, float y, float z);
	
private:
	static osg::ref_ptr< osg::Geometry > buildAxis();
	
	VisualizationUtils();
	virtual ~VisualizationUtils();
	
};

#endif /* VISUALIZATIONUTILS_HPP_ */
