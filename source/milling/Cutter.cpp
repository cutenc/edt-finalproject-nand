/*
 * Cutter.cpp
 *
 *  Created on: 03/set/2012
 *      Author: socket
 */

#include "Cutter.hpp"

#include <boost/make_shared.hpp>

#include "configuration/Geometry.hpp"
#include "cutters.hpp"

Cutter::Ptr Cutter::buildCutter(const CutterDescription &desc) {
	
	switch (desc.getGeometry()->getType()) {
		case Geometry::CYLINDER: {
			const Cylinder &geom = desc.getGeometry()->getAs< Cylinder >();
			return boost::make_shared< CylinderCutter >(geom, desc.getColor());
		}
		
		case Geometry::SPHERE: {
			const Sphere &geom = desc.getGeometry()->getAs< Sphere >();
			return boost::make_shared< SphereCutter >(geom, desc.getColor());
		}
		
		case Geometry::RECTANGULAR_CUBOID:
		case Geometry::MESH:
			throw std::runtime_error("Cutter geometry not registered");
			break;
	}
	
	throw std::invalid_argument("Cutter geometry not registered");
}

