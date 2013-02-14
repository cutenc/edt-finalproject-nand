/**
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

class Cutter;

/**
 * @class Cutter
 *
 * generic cutter, to be extended in spheric and cylindric cutter
 */
class Cutter : public Model3D {
	
public:
	
	typedef boost::shared_ptr< Cutter > Ptr;
	typedef boost::shared_ptr< const Cutter > ConstPtr;
	
private:
	
	Color color;
	
public:
	/**
	 * constructor
	 * @param color
	 */
	Cutter(Color color) : color(color) { }

	/**
	 * destructor
	 */
	virtual ~Cutter() { }
	
	/**
	 *
	 * @return the color
	 */
	const Color &getColor() const {
		return this->color;
	}
	
	/**
	 * setter for the color
	 * @param color
	 */
	void setColor(const Color &color) {
		this->color = color;
	}
	
	/**
	 * 
	 * @param point
	 * @return a value >= 0 if given point is inside the surface, a value < 0
	 * if given point is outside the surface. In order to optimize test
	 * performance, returned value cannot be used as the REAL distance between
	 * \c point and cutter surface
	 */
	virtual double getDistance(const Point3D &point) const {
		return getDistance(point.asVector());
	}
	
	/**
	 * 
	 * @param point in cutter basis (not its bounding box ones)
	 * @return >=0 => inside, <0 => outside, =0 PAY A LOT OF ATTENTION
	 */
	virtual double getDistance(const Eigen::Vector3d &point) const =0;
	
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

	/**
	 *
	 * @return Bounding Box infos
	 */
	virtual BoundingBoxInfo getBoundingBox() const =0;
	
	virtual std::ostream & toOutStream(std::ostream &os) const =0;
	
	/**
	 * overrides <<
	 * @param os
	 * @param c
	 * @return
	 */
	friend std::ostream & operator<<(std::ostream &os, const Cutter &c) {
		return c.toOutStream(os);
	}

	/**
	 * build the cutter
	 * @param desc
	 * @return
	 */
	static Cutter::Ptr buildCutter(const CutterDescription &desc);
	
};

#endif /* CUTTER_HPP_ */
