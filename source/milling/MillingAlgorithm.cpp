/*
 * MillingAlgorithm.cpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#include "MillingAlgorithm.hpp"

#include <stdexcept>

MillingAlgorithm::MillingAlgorithm(const ConfigFileParser &cfp, u_int maxOctreeDepth) :
		MAX_OCTREE_DEPTH(maxOctreeDepth),
		MOVE_IT(cfp.CNCMoveBegin()), MOVE_END(cfp.CNCMoveEnd()) {
}

MillingResult MillingAlgorithm::step() {
	if (hasFinished())
		throw std::runtime_error("Algorithm is already finished");
	
	this->stepNumber++;
	
	IntersectionResult infos = doIntersection(*MOVE_IT);
	
	(this->MOVE_IT)++;
	
	// TODO: activate water or not?
	bool water = false;
	
	return MillingResult(infos, water);
}

bool MillingAlgorithm::hasFinished() {
	return this->MOVE_IT == this->MOVE_END;
}

u_int MillingAlgorithm::getStepNumber() {
	return this->stepNumber;
}

IntersectionResult MillingAlgorithm::doIntersection(CNCMove move) {
	
	
	
}
