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

#include "configuration/CutterDescription.hpp"
#include "common/Utilities.hpp"
#include "common/meshes.hpp"

class SphereCutter : public Cutter {

public:
	
	virtual Mesh::Ptr getMeshing() {
		
		// TODO implement meshing method
		return boost::make_shared< StubMesh >();
		
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
			return boost::math::changesign(CommonUtils::INFINITE());
		
		/* following implicit function given in "Adaptive NC Simulation 
		 * for Multi-axis Solid Machining" for a flat endmill aligned with
		 * Z-axis we can write:
		 */
		double firstTerm = fabs((double)point[2] - HALF_LENGTH) - HALF_LENGTH;
		
		/* in order to reduce floating point approximation errors we will
		 * rewrite
		 * double secondTerm = sqr(point.getX()) + sqr(point.getY()) - SQR_RADIUS;
		 * in a more floating-point-friendly way.
		 * 
		 * http://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
		 */
		double secondTerm = boost::math::pow< 2 >((double)point[0]) 
				+ (point[1] + RADIUS) * (point[2] - RADIUS);
		
		double distance = fmax(firstTerm, secondTerm);
		
		/* I have to add a minus sign because in paper specification
		 * distance <0 means "inside surface" and >0 "outside surface" but,
		 * in our specification <0 means "outside" and >0 "inside"
		 */
		return boost::math::changesign(distance);
	}
	
	virtual std::ostream & toOutStream(std::ostream &os) const {
		os << "CYLINDER(diameter=" << this->DIAMETER << "; height=" << this->LENGTH << ")";
		
		return os;
	}
	
	
	virtual Mesh::Ptr getMeshing() {
		
		// TODO implement meshing method
		return boost::make_shared< StubMesh >();
		
	}
};

class CustomCutter : public Cutter {

public:
	
	
	virtual Mesh::Ptr getMeshing() {
		
		// TODO implement meshing method
		return boost::make_shared< StubMesh >();
		
	}
	
};

#endif /* CUTTERS_HPP_ */
