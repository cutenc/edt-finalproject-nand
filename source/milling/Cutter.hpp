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
#include "common/Model3D.hpp"
#include "configuration/CutterDescription.hpp"
#include "SimpleBox.hpp"

class Cutter;

class Cutter : public Model3D {
	
public:
	
	typedef boost::shared_ptr< Cutter > Ptr;
	typedef boost::shared_ptr< const Cutter > ConstPtr;
	
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
		BoundingBoxInfo(const Eigen::Vector3d &extents, const Eigen::Isometry3d &rototrasl) :
			extents(extents), rototraslation(rototrasl)
		{ }
		
		/**
		 * Extents of the bounding box (full extents not half ones)
		 */
		Eigen::Vector3d extents;
		
		/**
		 * Gives the rototraslation between Cutter origin (used as reference
		 * in each CNCMove) and SimpleBox centroid, in Cutter basis
		 */
		Eigen::Isometry3d rototraslation;
	};
	virtual BoundingBoxInfo getBoundingBox() const =0;
	
	virtual std::ostream & toOutStream(std::ostream &os) const =0;
	
	friend std::ostream & operator<<(std::ostream &os, const Cutter &c) {
		return c.toOutStream(os);
	}
	
	static Cutter::Ptr buildCutter(const CutterDescription &desc);
	
};

#endif /* CUTTER_HPP_ */
