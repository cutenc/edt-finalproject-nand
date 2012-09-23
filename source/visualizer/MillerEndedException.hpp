/*
 * MillerEnded.hpp
 *
 *  Created on: 23/set/2012
 *      Author: socket
 */

#ifndef MILLERENDEDEXCEPTION_HPP_
#define MILLERENDEDEXCEPTION_HPP_

#include <exception>

class MillerEndedException: public std::exception {
public:
	MillerEndedException() : std::exception() { }
	
	virtual ~MillerEndedException() throw() { }
};

#endif /* MILLERENDEDEXCEPTION_HPP_ */
