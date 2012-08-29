/*
 * cutters.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef CUTTERS_HPP_
#define CUTTERS_HPP_

#include <boost/shared_ptr.hpp>

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
	
	virtual double getDistance(const Point3D &point) const =0;
	
	struct BoundingBoxInfo {
		
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
	
};

class CustomCutter : public Cutter {
	
};


#endif /* CUTTERS_HPP_ */
