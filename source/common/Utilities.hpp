/**
 * Utilities.hpp
 *
 *  Created on: 17/ago/2012
 *      Author: socket
 */

#ifndef UTILITIES_HPP_
#define UTILITIES_HPP_

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <ios>

#include <Eigen/Geometry>

#include <osg/Geometry>

/**
 * @class FileUtils
 *
 * various tools for managing files
 */
class FileUtils {
	
public:
	
	struct ReadData {
		std::string validLine;
		std::streamoff lastReadPos;
		
		ReadData() { }
		ReadData(const std::string &line, const std::streamoff &lastPos) :
				validLine(line), lastReadPos(lastPos) { }
		virtual ~ReadData() { }
	};
	
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
	static ReadData readNextValidLine(std::istream &) throw(std::runtime_error);
	
};

/**
 * @class StringUtils
 *
 * Various tools for managing strings.
 */
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
	
	static std::string repeat(const std::string &pattern, unsigned int nTimes);
};


class GeometryUtils {
	
public:
	
	inline
	static void checkExtent(const Eigen::Vector3d extent) throw(std::invalid_argument) {
		double eps = std::numeric_limits<double>::epsilon();
		if (extent[0] < eps || extent[1] < eps || extent[2] < eps) {
			std::stringstream ss;
			ss << "some dimensions are too small or negative: [" << extent[0] <<
					", " << extent[1] << ", " << extent[2] << "]";
			
			throw std::invalid_argument(ss.str());
		}
	}
	
	inline
	static osg::Vec3d toOsg(const Eigen::Vector3d &vec) {
		return osg::Vec3d(vec(0), vec(1), vec(2));
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
	inline
	static bool isBetween(int x, int a, int b) {
		return (x >= a) && (x < b);
	}
	
	inline
	static double INFINITE() {
		static const double INF = (std::numeric_limits<double>::has_infinity) ? 
				std::numeric_limits<double>::infinity() : std::numeric_limits<double>::max();
		
		return INF;
	}
	
	inline
	static bool doubleEquals(double x, double y) {
		static double EPS = 3 * std::numeric_limits< double >::epsilon();
		
		return fabs(x - y) < EPS;
	}
};

#endif /* UTILITIES_HPP_ */
