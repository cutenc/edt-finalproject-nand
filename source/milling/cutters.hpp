/*
 * cutters.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef CUTTERS_HPP_
#define CUTTERS_HPP_

#include <limits>
#include <algorithm>
#include <cmath>

#include <boost/shared_ptr.hpp>

#include <Eigen/Geometry>

#include "configuration/CutterDescription.hpp"
#include "common/Color.hpp"
#include "common/Point3D.hpp"
#include "SimpleBox.hpp"

class Cutter;
typedef boost::shared_ptr< Cutter > CutterPtr;

class Cutter {
	
	Color color;
	
public:
	Cutter(Color color) : color(color) { }
	virtual ~Cutter() { }
	
	Color getColor() const {
		return this->color;
	}
	
	void setColor(const Color &color) {
		this->color = color;
	}
	
	/**
	 * 
	 * @param point
	 * @return >=0 => inside, <0 => outside
	 */
	virtual double getDistance(const Point3D &point) const =0;
	
	struct BoundingBoxInfo {
		BoundingBoxInfo(const Point3D &originTraslation, const SimpleBox &boundingBox) :
			originTraslation(originTraslation), boundingBox(boundingBox)
		{ }
		
		/**
		 * Gives the traslation vector between Cutter origin (used as reference
		 * in each CNCMove) and SimpleBox centroid
		 */
		Point3D originTraslation;
		
		
		SimpleBox boundingBox;
	};
	virtual BoundingBoxInfo getBoundingBox() const =0;
	
	static CutterPtr buildCutter(const CutterDescription &desc);
};

class SphereCutter : public Cutter {
	
};

class CylinderCutter : public Cutter {
	
	const double RADIUS;
	const double LENGTH;
	
	// used to speed up getDistance calculation
	const double HALF_LENGTH;
	const double SQR_RADIUS;
	
public:
	CylinderCutter(double radius, double length, const Color &color) :
		Cutter(color),
		RADIUS(radius), LENGTH(length),
		HALF_LENGTH(length / 2.0), SQR_RADIUS(sqr(radius)) {
		
		if (radius <= std::numeric_limits<double>::epsilon())
			throw std::invalid_argument("negative diameter (or too small)");
		if (length <= std::numeric_limits<double>::epsilon())
			throw std::invalid_argument("negative length (or too small)");
	}
	
	virtual BoundingBoxInfo getBoundingBox() const {
		SimpleBox bbox(Eigen::Vector3d(RADIUS, RADIUS, LENGTH));
		Eigen::Vector3d originTraslation(0, 0, LENGTH / 2.0);
		
		return BoundingBoxInfo(Point3D(originTraslation), bbox);
	}
	
	virtual double getDistance(const Point3D &point) const {
		/* following implicit function given in "Adaptive NC Simulation 
		 * for Multi-axis Solid Machining" for a flat endmill aligned with
		 * Z-axis we can write:
		 */
		double distance = std::max< double >(
				std::abs< double >(point.getZ() - HALF_LENGTH) - HALF_LENGTH,
				sqr(point.getX()) + sqr(point.getY()) - SQR_RADIUS
		);
		
		/* I have to add a minus sign because in paper specification
		 * distance <0 means "inside surface" and >0 "outside surface" but,
		 * in our specification <0 means "outside" and >0 "inside"
		 */
		return -distance;
	}
	
private:
	
	inline static double sqr(double x) {
		return x * x;
	}
	
};

class CustomCutter : public Cutter {
	
};

CutterPtr Cutter::buildCutter(const CutterDescription &desc) {
	
	switch (desc.getGeometry()->getType()) {
		case Geometry::CYLINDER: {
			
			// TODO
			
			break;
		}
	}
	
	throw std::invalid_argument("Cutter geometry not registered");
}


#endif /* CUTTERS_HPP_ */
