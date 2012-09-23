/*
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

struct MillingResult {
	
public:
	MillingResult(u_int stepNumber, const IntersectionResult &res, bool water) :
			stepNumber(stepNumber), intersection(res), water(water) { }
	
	u_int stepNumber;
	IntersectionResult intersection;
	bool water;
	
	static std::string getPrintHeader() {
		return "#move\twater(y/n)\t" + IntersectionResult::getPrintHeader();
	}
	
	friend std::ostream & operator<<(std::ostream &os, const MillingResult &mr) {
		os << mr.stepNumber << "\t" << ((mr.water) ? "y" : "n") << "\t" << mr.intersection;
		
		return os;
	}
	
};



#endif /* MILLINGRESULT_HPP_ */
