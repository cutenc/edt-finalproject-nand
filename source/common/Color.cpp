/*
 * Color.cpp
 *
 *  Created on: 14/ago/2012
 *      Author: socket
 */

#include "Color.hpp"

#include <cassert>

Color::Color() : color(1, 1, 1, 1)
{
}

Color::Color(const osg::Vec4f& color) : color(color)
{
}

Color::Color(const std::string& colorStr) : color(1, 1, 1, 1) 
{
	// TODO IMPLEMENT THIS METHOD, assigned color is just a stub
	assert(!colorStr.empty());
}


Color::~Color() {
}

const osg::Vec4f &Color::asOSG() const {
	return this->color;
}
