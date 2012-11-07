/*
 * Color.hpp
 *
 *  Created on: 14/ago/2012
 *      Author: socket
 */

#ifndef COLOR_HPP_
#define COLOR_HPP_


#include <osg/Geometry>

class Color {
	
private:
	osg::Vec4f color;
	
public:
	Color();
	
	/**
	 * Color string in the format 0xRRGGBB
	 * @param colorStr
	 */
	Color(const std::string &colorStr);
	Color(const osg::Vec4f &color);
	
	virtual ~Color();
	
	const osg::Vec4f &asOSG() const;
	
	/**
	 * Color string in the format 0xRRGGBB
	 * @param colorStr
	 * @return
	 */
	static osg::Vec4f buildColor(const std::string &colorStr);
};

#endif /* COLOR_HPP_ */
