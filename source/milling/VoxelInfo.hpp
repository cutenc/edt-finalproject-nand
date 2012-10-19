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

#include <osg/Geode>

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
	
	osg::ref_ptr<osg::Geode> geode;
	
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
	
	osg::ref_ptr<osg::Geode> getGeode() const;
	
	void setGeode(osg::ref_ptr<osg::Geode> geode);
	
	static double DEFAULT_INSIDENESS();
	
	static bool isInside(double d);

private:
	
	double getInsideness(u_char i) const;
	
	bool updateInsideness(u_char i, double newInsideness);

	bool updateInsideness(u_char i, double oldInsideness, double newInsideness);
	
};

#endif /* VOXELINFO_HPP_ */
