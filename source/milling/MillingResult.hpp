/*
 * MillingResults.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef MILLINGRESULT_HPP_
#define MILLINGRESULT_HPP_

struct MillingResult {
	
public:
	MillingResult(u_int stepNumber, const IntersectionResult &res, bool water) :
			stepNumber(stepNumber), intersection(res), water(water) { }
	
	u_int stepNumber;
	IntersectionResult intersection;
	bool water;
	
	friend std::ostream & operator<<(std::ostream &os, const MillingResult &mr) {
		os << "#" << mr.stepNumber << " [" << ((mr.water) ? "X" : " ") << "] water" << std::endl;
		os << mr.intersection;
		
		return os;
	}
	
};



#endif /* MILLINGRESULT_HPP_ */
