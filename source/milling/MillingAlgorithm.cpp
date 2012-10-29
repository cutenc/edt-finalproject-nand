/*
 * MillingAlgorithm.cpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#include "MillingAlgorithm.hpp"

#include <cmath>
#include <stdexcept>

#include <boost/make_shared.hpp>

#include <Eigen/Geometry>

#include "common/constants.hpp"
#include "common/Point3D.hpp"
#include "common/EulerAngles.hpp"
#include "Cutter.hpp"
#include "Stock.hpp"

MillingAlgorithm::MillingAlgorithm(const MillingAlgorithmConf &conf) :
		CONFIG(conf)
{
	this->waterFluxWasteCount = 0;
	this->stepNumber = 0;
}

MillingAlgorithm::~MillingAlgorithm() { }

MillingAlgorithm::StepInfo MillingAlgorithm::step() {
	assert(hasNextStep());
	
	this->stepNumber++;
	
	const CNCMove &move = *CONFIG.MOVE_IT;
	IntersectionResult infos = doIntersection(move);
	
	bool water = false;
	this->waterFluxWasteCount += infos.waste;
	if (this->waterFluxWasteCount > CONFIG.waterThreshold) {
		this->waterFluxWasteCount = std::max< double >(0, 
				this->waterFluxWasteCount - CONFIG.waterFlux);
		water = true;
	}
			
	++(CONFIG.MOVE_IT);
	return StepInfo(MillingResult(this->stepNumber, infos, water), move);
}

bool MillingAlgorithm::hasNextStep() {
	return CONFIG.MOVE_IT != CONFIG.MOVE_END;
}

unsigned int MillingAlgorithm::getStepNumber() {
	return this->stepNumber;
}

Eigen::Vector3d MillingAlgorithm::getResolution() const {
	return CONFIG.STOCK->getResolution();
}

IntersectionResult MillingAlgorithm::doIntersection(const CNCMove &move) {
	
	/* given move rototraslations are in respect of world basis so we have
	 * to "merge" this two informations in order to find cutter rototraslation
	 * in stock basis
	 */
	
	/* when returned as eigen both these isometry are matrices that
	 * converts point from stock/cutter coords to world ones: i.e. they
	 * convert from stock/cutter basis to world basis; we have to create 
	 * an isometry that converts from cutter coords to stock coords.
	 * 
	 * P_world = StockIsom_world * P_stock
	 * P'_world = CutterIsom_world * P'_cutter
	 * P''_stock = inverse(StockIsom_world) * P''_world
	 * 
	 * P3_stock = inverse(StockIsom_world) * CutterIsom_world * P3_cutter
	 */
	Eigen::Isometry3d cutterIsom_stock = move.STOCK.asEigen().inverse() * move.CUTTER.asEigen();
	
	return CONFIG.STOCK->intersect(CONFIG.CUTTER, cutterIsom_stock);
}

std::ostream& operator <<(std::ostream& os, const MillingAlgorithm& ma) {
	os << "MILLING_ALGORITHM(currStep#:" << ma.stepNumber << "; next_move:";
	if (ma.CONFIG.MOVE_IT == ma.CONFIG.MOVE_END)
		os << "ENDED";
	else
		os << *(ma.CONFIG.MOVE_IT);
	os << ")" << std::endl
			<< "\tCutter: " << *ma.CONFIG.CUTTER //; ma.cutter->toOutStream(os)
			<< std::endl
			<< "\tStock: " << *ma.CONFIG.STOCK;
	os << std::endl << "END_MILLING_ALGORITHM";
	
	return os;
}
