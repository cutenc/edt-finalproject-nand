/*
 * cutters.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef CUTTERS_HPP_
#define CUTTERS_HPP_

#include "Cutter.hpp"

#include <limits>
#include <algorithm>
#include <cmath>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/math/special_functions.hpp>

#include <Eigen/Geometry>

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/ShapeDrawable>

#include "configuration/CutterDescription.hpp"
#include "common/Utilities.hpp"
#include "visualizer/VisualizationUtils.hpp"

class SphereCutter : public Cutter {

private:
	const double RADIUS;
	const double DIAMETER;
	const double SQUARE_RADIUS;
	
public:
	SphereCutter(const Sphere &geom, const Color &color):
		Cutter(color), RADIUS(geom.RADIUS), DIAMETER(RADIUS * 2.0),
		SQUARE_RADIUS(RADIUS * RADIUS)
	{
		if (RADIUS <= std::numeric_limits<double>::epsilon())
			throw std::invalid_argument("negative diameter (or too small)");
	}
	
	
	virtual BoundingBoxInfo getBoundingBox() const {
		static const Eigen::Vector3d EXTENTS(DIAMETER, DIAMETER, DIAMETER);
		return BoundingBoxInfo(EXTENTS, Eigen::Isometry3d::Identity());
	}
	
	virtual double getDistance(const Eigen::Vector3d &point) const {
		/* in our specification <0 means "outside" and >0 "inside"
		 */
		
//		double squaredNorm = 0;
//		for (int i = 0; i < 3; ++i) {
//			squaredNorm += boost::math::pow<2>((double)point[i]);
//		}
		
		return SQUARE_RADIUS - point.squaredNorm();
	}
	
	virtual std::ostream & toOutStream(std::ostream &os) const {
		os << "SPHERE(diameter=" << this->DIAMETER << ")";
		
		return os;
	}
	
	virtual Mesh::Ptr getMeshing() {
		static const Mesh::Ptr mesh = buildMesh();
		
		return mesh;
	}
	
private:
	Mesh::Ptr buildMesh() const {
	
		osg::ref_ptr< osg::ShapeDrawable > sphere = VisualizationUtils::buildSphere(RADIUS);
		sphere->setColor(getColor().asOSG());
		
		osg::ref_ptr< osg::Geode > geode = new osg::Geode;
		geode->addDrawable(sphere.get());
		
		return boost::make_shared< Mesh >(geode.get());
	}
	
};

class CylinderCutter : public Cutter {
	
	const double RADIUS;
	const double LENGTH;
	
	// used to speed up getDistance calculation
	const double HALF_LENGTH;
	const double DIAMETER;
	
public:
	CylinderCutter(const Cylinder &geom, const Color &color) :
		Cutter(color),
		RADIUS(geom.RADIUS), LENGTH(geom.HEIGHT),
		HALF_LENGTH(geom.HEIGHT * 0.5), DIAMETER(geom.RADIUS * 2)
	{
		
		if (RADIUS <= std::numeric_limits<double>::epsilon())
			throw std::invalid_argument("negative diameter (or too small)");
		// i need that half_length has an appreciable size
		if (HALF_LENGTH <= std::numeric_limits<double>::epsilon())
			throw std::invalid_argument("negative length (or too small)");
	}
	
	virtual BoundingBoxInfo getBoundingBox() const {
		
		Eigen::Vector3d extents(DIAMETER, DIAMETER, LENGTH);
		Eigen::Translation3d originTraslation(0, 0, HALF_LENGTH);
		
		return BoundingBoxInfo(extents, Eigen::Isometry3d(originTraslation));
	}
	
	virtual double getDistance(const Eigen::Vector3d &point) const {
		if (point[2] < 0)
			return -1;
		
		/* Optimization trials:
		 * - tryed to optimize away the calculation of one of the terms
		 * by checking wether firstTerm is already outside (exploiting fmax
		 * behavior); it seems that introduced 'if' (i.e. if (firstTerm > 0) )
		 * breaks pipelining very often resulting in worse overall
		 * performance (+0.40)
		 * EDIT: rimesso perchè sembra cambiare qualcosina in meglio... :)
		 */
		
		/* following implicit function given in "Adaptive NC Simulation 
		 * for Multi-axis Solid Machining" for a flat endmill aligned with
		 * Z-axis we can write:
		 */
		double firstTerm = fabs(point(2) - HALF_LENGTH) - HALF_LENGTH;
		
		if (firstTerm >= 0)
			return -firstTerm;
		
		/* in order to reduce floating point approximation errors we will
		 * rewrite
		 * double secondTerm = sqr(point.getX()) + sqr(point.getY()) - SQR_RADIUS;
		 * in a more floating-point-friendly way.
		 * 
		 * http://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
		 */
		double secondTerm = boost::math::pow< 2 >(point[0])
				+ (point[1] + RADIUS) * (point[1] - RADIUS);
		
		
//		double distance = std::max(firstTerm, secondTerm);
		
		/* I have to add a minus sign because in paper specification
		 * distance <0 means "inside surface" and >0 "outside surface" but,
		 * in our specification <0 means "outside" and >0 "inside"
		 */
		return -secondTerm;
	}
	
	virtual std::ostream & toOutStream(std::ostream &os) const {
		os << "CYLINDER(diameter=" << this->DIAMETER << "; height=" << this->LENGTH << ")";
		
		return os;
	}
	
	
	virtual Mesh::Ptr getMeshing() {
		static const Mesh::Ptr mesh = buildMesh();
		
		return mesh;
	}
	
private:
	Mesh::Ptr buildMesh() const {
		/* our center is on the center of the below base so we have to draw a 
		 * translated cylinder
		 */
		osg::Vec3 tras(0, 0, HALF_LENGTH);
		osg::ref_ptr< osg::ShapeDrawable > cylDraw = VisualizationUtils::buildCylinder(tras, RADIUS, LENGTH);
		cylDraw->setColor(getColor().asOSG());
		
		osg::ref_ptr< osg::Geode > geode = new osg::Geode;
		geode->addDrawable(cylDraw.get());
		
		return boost::make_shared< Mesh >(geode.get());
	}
};

class CustomCutter : public Cutter {

public:
	
	
	virtual Mesh::Ptr getMeshing() {
		
		// TODO implement meshing method
		throw std::runtime_error("NOT IMPLEMENTED YET");
		
	}
	
};

#endif /* CUTTERS_HPP_ */
