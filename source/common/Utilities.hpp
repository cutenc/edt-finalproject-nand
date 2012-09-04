/*
 * Utilities.hpp
 *
 *  Created on: 17/ago/2012
 *      Author: socket
 */

#ifndef UTILITIES_HPP_
#define UTILITIES_HPP_

#include <iostream>
#include <stdexcept>

#include <bits/postypes.h>

#include <Eigen/Geometry>

typedef struct _readData {
	std::string validLine;
	std::streampos lastReadPos;
} readData;

class FileUtils {
	
public:
	
	static void openFile(const std::string &filename, std::ifstream &ifs);
	
	/**
	 * Checks if given string can be skipped during the parsing of the
	 * configuration file. Line will be trimmed inside the method.
	 * @param line
	 * @return
	 */
	static bool isSkippableLine(std::string &line);
	
	/**
	 * Read and return a valid line from given std::ifstream
	 * @param 
	 * @return
	 * 
	 * @throw std::runtime_error if there's no other valid lines in the given
	 * stream.
	 */
	static readData readNextValidLine(std::istream &) throw(std::runtime_error);
	
};

class StringUtils {
	
public:
	/**
	 * Returns the result of the match of given \c line against the reg.exp.
	 * pattern formed as follow: 
	 * @code
	 * 	propName + "\\s*=\\s*(" + propValuePatter + ")"
	 * @endcode
	 * 
	 * @param line
	 * @param propName
	 * @param propValuePattern
	 * @param icase when enabled (default) perform a case-insensitive match.
	 * @return
	 * 
	 * @throw std::runtime_error when given line does not match.
	 */
	static std::string extractProperty(const std::string &line, const std::string &propName, 
			const std::string &propValuePattern, bool icase = true) throw(std::runtime_error);
	
	static std::string repeat(std::string pattern, u_int nTimes);
};


class GeometryUtils {
	
public:
	
	static void checkExtent(const Eigen::Vector3d extent) throw(std::invalid_argument) {
		double eps = std::numeric_limits<double>::epsilon();
		if (extent[0] < eps || extent[1] < eps || extent[2] < eps) {
			std::stringstream ss;
			ss << "some dimensions are too small or negative: [" << extent[0] <<
					", " << extent[1] << ", " << extent[2] << "]";
			
			throw std::invalid_argument(ss.str());
		}
	}
	
};


class CommonUtils {
	
public:
	
	/**
	 * True if x in [a, b[
	 * @param a
	 * @param b
	 * @return
	 */
	static bool isBetween(int x, int a, int b) {
		return (x >= a) && (x < b);
	}
	
	static const double INFINITE;
};

#endif /* UTILITIES_HPP_ */
