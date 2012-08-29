/*
 * MillingAlgorithm.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef MILLINGALGORITHM_HPP_
#define MILLINGALGORITHM_HPP_

#include "configuration/ConfigFileParser.hpp"
#include "cutters.hpp"
#include "Stock.hpp"

struct MillingResult {
	
public:
	MillingResult(IntersectionResult res, bool water) :
			intersection(res), water(water) { }
	
	IntersectionResult intersection;
	bool water;
};

class MillingAlgorithm {
	
	const u_int MAX_OCTREE_DEPTH;
	CNCMoveIterator MOVE_IT, MOVE_END;
	
	u_int stepNumber;
	
public:
	MillingAlgorithm(const ConfigFileParser &cfp, u_int maxOctreeDepth);
	virtual ~MillingAlgorithm();
	
	MillingResult step();
	
	bool hasFinished();
	u_int getStepNumber();
	
private:
	
	IntersectionResult doIntersection(CNCMove move);
	
};

#endif /* MILLINGALGORITHM_HPP_ */
