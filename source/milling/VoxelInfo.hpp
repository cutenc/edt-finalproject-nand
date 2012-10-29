/*
 * MeshingInfo.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef VOXELINFO_HPP_
#define VOXELINFO_HPP_

#include <ostream>
#include <cassert>

#include <boost/shared_ptr.hpp>

#include "common/Utilities.hpp"
#include "Corner.hpp"
#include "graphics_info.hpp"


class VoxelInfo {
	
public:
	typedef boost::shared_ptr< VoxelInfo > Ptr;
	typedef boost::shared_ptr< const VoxelInfo > ConstPtr;
	
private:
	// updated by updateInsideness(unsigned char, double) function
	unsigned char insideCorners;
	
	GraphicPointer graphics;
	
public:
	VoxelInfo(double val);
	
	virtual ~VoxelInfo();
	
	inline
	bool isIntersecting() const {
		return insideCorners > 0;
	}
	
	inline
	bool isContained() const {
		return insideCorners == 0xff;
	}
	
	inline
	bool isCornerCut(Corner::CornerType c) const {
		int i = static_cast<int>(c);
		
		assert(i < 8); // MUST be 8: see shift operation
		
		return insideCorners & (0x01 << i);
	}
	
	/**
	 * 
	 * @param c
	 * @param insideness
	 * @return \c true if the corner was previously outside but now is inside,
	 * \c false otherwise.
	 */
	inline
	bool updateInsideness(Corner::CornerType c, double newInsideness) {
		
		unsigned char oldInside = insideCorners;
		
		int i = static_cast< int >(c);
		assert(i >= 0 && i < 8);
		
		/* the following statement will set to 1 the correct bit only if isInside
		 * returns true. This is true because quoting from the standard about 
		 * integral promotions (4.5):
		 * "An rvalue of type bool can be converted to an rvalue of type int, 
		 * with false becoming zero and true becoming one."
		 */
		insideCorners |= ((int)isInside(newInsideness)) << i;
		
		/* if no bits were changed, XOR will return 0 meaning no changes, otherwise
		 * it will return a value != 0 meaning that a corner fell inside: pay
		 * attention that due to the previous OR operation, a corner cannot "fall
		 * outside" that is make a bit change from 1 to 0.
		 */
		return oldInside ^ insideCorners;
	}
	
	friend std::ostream & operator<<(std::ostream &os, const VoxelInfo &vinfo);
	
	const GraphicPointer &getGraphics() const;
	
	void setGraphics(const GraphicPointer &graphics);
	
	bool hasGraphics() const;
	
	inline
	static double DEFAULT_INSIDENESS() {
		static const double DEF_INSIDENESS = -CommonUtils::INFINITE();
		return DEF_INSIDENESS;
	}
	
	inline
	static bool isInside(double d) {
		// -0,0 because -0.0 < 0.0
		return d >= -0.0;
	}

};

#endif /* VOXELINFO_HPP_ */
