/*
 * MeshingInfo.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef VOXELINFO_HPP_
#define VOXELINFO_HPP_

#include <ostream>
#include <limits>
#include <algorithm>
#include <cassert>
#include <cmath>

#include <boost/shared_ptr.hpp>

#include <osg/Node>

#include "Corner.hpp"
#include "common/Utilities.hpp"

class VoxelInfo {
	
public:
	typedef boost::shared_ptr< VoxelInfo > Ptr;
	typedef boost::shared_ptr< const VoxelInfo > ConstPtr;
	
private:
	// updated by updateInsideness(u_char, double) function
	u_char insideCorners;
	double insideness[Corner::N_CORNERS];
	
	osg::ref_ptr<osg::Node> graphics;
	
public:
	
	VoxelInfo(double val);
	
	virtual ~VoxelInfo();
	
	bool isIntersecting() const;
	
	bool isContained() const;
	
	bool isCornerCut(Corner::CornerType c) const;
	
	double getInsideness(Corner::CornerType c) const;
	
	/**
	 * 
	 * @param c
	 * @param insideness
	 * @return \c true if the carner was previously outside but now is inside,
	 * \c false otherwise.
	 */
	bool updateInsideness(Corner::CornerType c, double insideness);
	
	friend std::ostream & operator<<(std::ostream &os, const VoxelInfo &vinfo);
	
	osg::ref_ptr<osg::Node> getGraphics() const;
	
	void setGraphics(osg::ref_ptr<osg::Node> geode);
	
	bool hasGraphics() const;
	
	static double DEFAULT_INSIDENESS();
	
	static bool isInside(double d);

private:
	
	double getInsideness(u_char i) const;
	
};

#endif /* VOXELINFO_HPP_ */
