/**
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
#include "Cutter.hpp"
#include "Stock.hpp"
#include "MillingResult.hpp"
#include "MillingAlgorithmConf.hpp"

/**
 * executes milling, one step a time
 * @class MillingAlgorithm
 */
class MillingAlgorithm {

public:
	typedef std::pair<MillingResult, CNCMove> StepInfo;
	typedef boost::shared_ptr< MillingAlgorithm > Ptr;
	
private:
	MillingAlgorithmConf CONFIG;
	
	double waterFluxWasteCount;
	unsigned int stepNumber;
	
	
public:
	/**
	 * constructor
	 * @param config
	 */
	MillingAlgorithm(const MillingAlgorithmConf &config);
	
	virtual ~MillingAlgorithm();
	
	/**
	 * advances milling one step a time
	 * @return
	 */
	StepInfo step();
	
	/**
	 * checks whether milling can continue (True), or it is ended (False).
	 * @return boolean
	 */
	bool hasNextStep();
	
	unsigned int getStepNumber();
	
	/**
	 * Returns dimensions of the smallest voxel in which STOCK will be divided.
	 * @return
	 */
	Eigen::Vector3d getResolution() const;
	
	// MeshingInfo buildCurrentMeshingInfo();
	
	/**
	 * redefines operator << for print
	 */
	friend std::ostream & operator<<(std::ostream &os, const MillingAlgorithm &ma);
	
private:
	
	IntersectionResult doIntersection(const CNCMove &move);
	
};

#endif /* MILLINGALGORITHM_HPP_ */
