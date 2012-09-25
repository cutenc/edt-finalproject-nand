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

#include <Eigen/Geometry>

#include "configuration/CutterDescription.hpp"
#include "common/meshes.hpp"
#include "SimpleBox.hpp"

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
	const double SQR_RADIUS;
	
public:
	CylinderCutter(const Cylinder &geom, const Color &color) :
		Cutter(color),
		RADIUS(geom.RADIUS), LENGTH(geom.HEIGHT),
		HALF_LENGTH(geom.HEIGHT / 2.0), DIAMETER(geom.RADIUS * 2),
		SQR_RADIUS(sqr(geom.RADIUS)) {
		
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
	
	virtual double getDistance(const Point3D &point) const {
		if (point.getZ() < 0)
			return -CommonUtils::INFINITE();
		
		/* following implicit function given in "Adaptive NC Simulation 
		 * for Multi-axis Solid Machining" for a flat endmill aligned with
		 * Z-axis we can write:
		 */
		double distance = std::max< double >(
				fabs(point.getZ() - HALF_LENGTH) - HALF_LENGTH,
				sqr(point.getX()) + sqr(point.getY()) - SQR_RADIUS
		);
		
		/* I have to add a minus sign because in paper specification
		 * distance <0 means "inside surface" and >0 "outside surface" but,
		 * in our specification <0 means "outside" and >0 "inside"
		 */
		return -distance;
	}
	
	virtual std::ostream & toOutStream(std::ostream &os) const {
		os << "CYLINDER(diameter=" << this->DIAMETER << "; height=" << this->LENGTH << ")";
		
		return os;
	}
	
	
	virtual Mesh::Ptr getMeshing() {
		
		// TODO implement meshing method
		return boost::make_shared< StubMesh >();
		
	}
	
private:
	
	inline static double sqr(double x) {
		return x * x;
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
