/*
 * Cutter.hpp
 *
 *  Created on: 03/set/2012
 *      Author: socket
 */

#ifndef CUTTER_HPP_
#define CUTTER_HPP_

#include <boost/shared_ptr.hpp>

#include "common/Color.hpp"
#include "common/Point3D.hpp"
#include "configuration/CutterDescription.hpp"
#include "SimpleBox.hpp"

class Cutter;

class Cutter {
	
public:
	
	typedef boost::shared_ptr< Cutter > CutterPtr;
	
private:
	
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
	 * @param point in cutter basis (not its bounding box ones)
	 * @return >=0 => inside, <0 => outside
	 */
	virtual double getDistance(const Point3D &point) const =0;
	
	struct BoundingBoxInfo {
		BoundingBoxInfo(const Point3D &originTraslation, const SimpleBox &boundingBox) :
			originTraslation(originTraslation), boundingBox(boundingBox)
		{ }
		
		/**
		 * Gives the traslation vector between Cutter origin (used as reference
		 * in each CNCMove) and SimpleBox centroid, ni Cutter basis
		 */
		Point3D originTraslation;
		
		
		SimpleBox boundingBox;
	};
	virtual BoundingBoxInfo getBoundingBox() const =0;
	
	static CutterPtr buildCutter(const CutterDescription &desc);
};

#endif /* CUTTER_HPP_ */
