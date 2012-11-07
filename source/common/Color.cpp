/*
 * Color.cpp
 *
 *  Created on: 14/ago/2012
 *      Author: socket
 */

#include "Color.hpp"

#include <cassert>

#include <sstream>

Color::Color() : color(1, 1, 1, 1)
{
}

Color::Color(const osg::Vec4f& color) : color(color)
{
}

Color::Color(const std::string& colorStr) : color(buildColor(colorStr))
{ }


Color::~Color() {
}

const osg::Vec4f &Color::asOSG() const {
	return this->color;
}

osg::Vec4f Color::buildColor(const std::string& colorStr) {
	assert(colorStr.length() == 8);
	
	unsigned int color;
	
	std::stringstream ss;
	ss << std::hex << colorStr.substr(2);
	ss >> color;
	
	unsigned char red = (color & 0xff0000u) >> 16;
	unsigned char green = (color & 0x00ff00u) >> 8;
	unsigned char blue = (color & 0x0000ffu);
	
	return osg::Vec4f(
			red / 255.f,
			green / 255.f,
			blue / 255.f,
			1.f
	);
}

