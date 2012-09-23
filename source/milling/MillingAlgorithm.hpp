/*
 * MillingAlgorithm.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef MILLINGALGORITHM_HPP_
#define MILLINGALGORITHM_HPP_

#include <ostream>
#include <utility>

#include <boost/shared_ptr.hpp>

#include "configuration/ConfigFileParser.hpp"
#include "meshing/MeshingInfo.hpp"
#include "Cutter.hpp"
#include "Stock.hpp"
#include "MillingResult.hpp"
#include "MillingAlgorithmConf.hpp"

class MillingAlgorithm {

public:
	typedef std::pair<MillingResult, CNCMove> StepInfo;
	typedef boost::shared_ptr< MillingAlgorithm > Ptr;
	
private:
	MillingAlgorithmConf CONFIG;
	
	double waterFluxWasteCount;
	u_int stepNumber;
	
	
public:
	MillingAlgorithm(const MillingAlgorithmConf &config);
	
	virtual ~MillingAlgorithm();
	
	StepInfo step();
	
	bool hasFinished();
	
	u_int getStepNumber();
	
	/**
	 * Returns dimensions of the smallest voxel in which STOCK will be divided.
	 * @return
	 */
	Eigen::Vector3d getResolution() const;
	
	// MeshingInfo buildCurrentMeshingInfo();
	
	friend std::ostream & operator<<(std::ostream &os, const MillingAlgorithm &ma);
	
private:
	
	IntersectionResult doIntersection(const CNCMove &move);
	
};

#endif /* MILLINGALGORITHM_HPP_ */
