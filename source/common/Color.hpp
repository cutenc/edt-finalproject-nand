/*
 * Color.hpp
 *
 *  Created on: 14/ago/2012
 *      Author: socket
 */

#ifndef COLOR_HPP_
#define COLOR_HPP_

#include <string>

class Color {
public:
	Color();
	
	/**
	 * Accept a color string given in HTML-hex format: \c 0xRRGGBB
	 * 
	 * @param color
	 */
	Color(const std::string &color);
	virtual ~Color();
};

#endif /* COLOR_HPP_ */
