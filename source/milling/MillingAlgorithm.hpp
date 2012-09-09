/*
 * MillingAlgorithm.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef MILLINGALGORITHM_HPP_
#define MILLINGALGORITHM_HPP_

#include <ostream>

#include "configuration/ConfigFileParser.hpp"
#include "Cutter.hpp"
#include "Stock.hpp"
#include "MillingResult.hpp"

class MillingAlgorithm {
	
	const u_int MAX_OCTREE_DEPTH;
	CNCMoveIterator MOVE_IT, MOVE_END;
	
	double waterFluxWasteCount;
	u_int stepNumber;
	
	Cutter::CutterPtr cutter;
	Stock::StockPtr stock;
	
public:
	MillingAlgorithm(const ConfigFileParser &cfp, u_int maxOctreeDepth);
	virtual ~MillingAlgorithm();
	
	MillingResult step();
	
	bool hasFinished();
	
	u_int getStepNumber();
	
	/**
	 * Returns dimensions of the smallest voxel in which STOCK will be divided.
	 * @return
	 */
	Eigen::Vector3d getResolution() const;
	
	MeshingInfo buildCurrentMeshingInfo();
	
	friend std::ostream & operator<<(std::ostream &os, const MillingAlgorithm &ma);
	
private:
	
	IntersectionResult doIntersection(const CNCMove &move);
	
};

#endif /* MILLINGALGORITHM_HPP_ */
