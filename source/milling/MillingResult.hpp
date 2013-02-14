/**
 * MillingResults.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef MILLINGRESULT_HPP_
#define MILLINGRESULT_HPP_

#include <sstream>

#include <boost/shared_ptr.hpp>

#include "Stock.hpp"

/**
 * @class MillingResult
 *
 * infos gathered every milling operation
 */
struct MillingResult {
	
public:
	/**
	 * constructor
	 *
	 * @param stepNumber
	 * @param res
	 * @param water
	 */
	MillingResult(unsigned int stepNumber, const IntersectionResult &res, bool water) :
			stepNumber(stepNumber), intersection(res), water(water) { }
	
	/** which step it is referred to */
	unsigned int stepNumber;

	/** the intersection results */
	IntersectionResult intersection;

	/** does the water flux have to be triggered? */
	bool water;
	
	/**
	 *
	 * @return string containing infos
	 */
	static std::string getPrintHeader() {
		return "#move\twater(y/n)\t" + IntersectionResult::getPrintHeader();
	}
	
	/**
	 * overrides << op.
	 * @param os
	 * @param mr
	 * @return
	 */
	friend std::ostream & operator<<(std::ostream &os, const MillingResult &mr) {
		os << mr.stepNumber << "\t" << ((mr.water) ? "y" : "n") << "\t" << mr.intersection;
		
		return os;
	}
	
};



#endif /* MILLINGRESULT_HPP_ */
