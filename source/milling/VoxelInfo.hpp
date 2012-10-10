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

#include "Corner.hpp"
#include "common/Utilities.hpp"

class VoxelInfo {
	
private:
	// updated by updateInsideness(u_char, double) function
	u_char insideCornerNumber;
	double insideness[Corner::N_CORNERS];
	
public:
	
	/**
	 * This constructor leave object in a incosistent state and should be
	 * used only in conjunction with the #setInsideness method. It is
	 * provided just to increase performance avoiding useless initializations.
	 */
	VoxelInfo();
	
	VoxelInfo(double val);
	
	virtual ~VoxelInfo();
	
	bool isIntersecting() const;
	
	bool isContained() const;
	
	bool isInside(Corner::CornerType c) const;

	u_char getInsideCornersNumber() const;
	
	double getInsideness(Corner::CornerType c) const;
	
	/**
	 * You should use this method only in combination with the empty constructor
	 * to initialize all corners of this object, but not for future
	 * manipulation. Wrong usage of this method may result in current object
	 * misbehaviour (in particular inside corner count may be incorrect,
	 * affecting also methods like #isContained and #isIntersecting)
	 * 
	 * @param c
	 * @param insideness
	 * @return \c true if the carner fall inside (according to #isInside)
	 * 
	 * @see #updateInsideness
	 */
	bool setInsideness(Corner::CornerType c, double insideness);
	
	/**
	 * You should use this method only in combination with the empty constructor
	 * to initialize all corners of this object, but not for future
	 * manipulation. Wrong usage of this method may result in current object
	 * misbehaviour (in particular inside corner count may be incorrect,
	 * affecting also methods like #isContained and #isIntersecting)
	 * 
	 * @param c
	 * @param insideness
	 * @return \c true if the carner fall inside (according to #isInside)
	 * 
	 * @see #updateInsideness
	 */
	bool setInsideness(Corner::CornerType c, double oldInsideness, double newInsideness);
	
	/**
	 * Resets the number of inside corners to 0; you should call this
	 * method only if you are going to call #setInsideness on each corner
	 * after that.
	 * 
	 * @see #updateInsideness
	 */
	void reset();
	
	/**
	 * 
	 * @param c
	 * @param insideness
	 * @return \c true if the carner was previously outside but now is inside,
	 * \c false otherwise.
	 */
	bool updateInsideness(Corner::CornerType c, double insideness);
	
	VoxelInfo & operator+=(const VoxelInfo &v);
	
	const VoxelInfo operator+(const VoxelInfo &v);
	
	friend std::ostream & operator<<(std::ostream &os, const VoxelInfo &vinfo);
	
	static double DEFAULT_INSIDENESS();
	
	static bool isInside(double d);
	
private:
	
	double getInsideness(u_char i) const;
	
	bool setInsideness(u_char i, double newInsideness);
	
	bool setInsideness(u_char i, double oldInsideness, double newInsideness);
	
	bool updateInsideness(u_char i, double newInsideness);

	bool updateInsideness(u_char i, double oldInsideness, double newInsideness);
	
};

#endif /* VOXELINFO_HPP_ */
