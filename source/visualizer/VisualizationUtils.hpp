/**
 * @file VisualizationUtils.hpp
 *
 * Created on: 20/ott/2012
 * Author: socket
 *
 * various utilities for the visualizer
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

	/**
	 * builds a cylinder, given the origin and its dimensions
	 *
	 * @param origin : the base point to calculate where to place the cylinder
	 * @param radius : radius of the base of the cylinder
	 * @param height : height of the cylinder
	 * @return the pointer of the cylinder
	 */
	static osg::ref_ptr< osg::ShapeDrawable > buildCylinder(const osg::Vec3 &origin, float radius, float height);
	/**
	 * builds a cylinder, given only its dimensions
	 *
	 * @param radius : radius of the base of the cylinder
	 * @param height : height of the cylinder
	 * @return the pointer of the cylinder
	 */
	static osg::ref_ptr< osg::ShapeDrawable > buildCylinder(float radius, float height);

	/**
	 * builds a sphere, given the origin and its radius
	 *
	 * @param origin : the base point to calculate where to place the sphere
	 * @param radius : radius of the sphere
	 * @return the pointer of the sphere
	 */
	static osg::ref_ptr< osg::ShapeDrawable > buildSphere(const osg::Vec3 &origin, float radius);
	/**
	 * builds a sphere, given only its radius
	 *
	 * @param radius : radius of the sphere
	 * @return the pointer of the sphere
	 */
	static osg::ref_ptr< osg::ShapeDrawable > buildSphere(float radius);

	/**
	 * builds a box (parallelepiped), given the origin and its radius
	 *
	 * @param origin : the base point to calculate where to place the parallelepiped
	 * @param x : width of the parallelepiped
	 * @param y : depth of the parallelepiped
	 * @param z : height of the parallelepiped
	 * @return the pointer of the sphere
	 */
	static osg::ref_ptr< osg::ShapeDrawable > buildBox(const osg::Vec3 &origin, float x, float y, float z);
	/**
	 * builds a box (parallelepiped), given only its radius
	 *
	 * @param x : width of the parallelepiped
	 * @param y : depth of the parallelepiped
	 * @param z : height of the parallelepiped
	 * @return the pointer of the sphere
	 */
	static osg::ref_ptr< osg::ShapeDrawable > buildBox(float x, float y, float z);
	
private:
	/**
	 * builds the axis system
	 *
	 * @return the pointer to the OSG geometric object representing the axis
	 */
	static osg::ref_ptr< osg::Geometry > buildAxis();
	
	/**
	 * constructor - empty
	 */
	VisualizationUtils();

	/**
	 * destructor - empty
	 */
	virtual ~VisualizationUtils();
	
};

#endif /* VISUALIZATIONUTILS_HPP_ */
