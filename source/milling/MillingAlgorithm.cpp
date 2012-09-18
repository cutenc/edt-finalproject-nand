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

MillingAlgorithm::MillingAlgorithm(Stock::Ptr stock, Cutter::ConstPtr cutter,
		const CNCMoveIterator &begin, const CNCMoveIterator &end) :
		STOCK(stock), CUTTER(cutter), MOVE_IT(begin), MOVE_END(end)
{
	this->waterFluxWasteCount = 0;
	this->stepNumber = 0;
}

MillingAlgorithm::~MillingAlgorithm() { }

MillingResult MillingAlgorithm::step() {
	if (hasFinished())
		throw std::runtime_error("Algorithm is already finished");
	
	this->stepNumber++;
	
	const CNCMove &move = *MOVE_IT;
	IntersectionResult infos = doIntersection(move);
	
	bool water = false;
	this->waterFluxWasteCount += infos.waste;
	if (this->waterFluxWasteCount > ALG_WATER_THRESHOLD) {
		this->waterFluxWasteCount = std::max< double >(0, 
				this->waterFluxWasteCount - ALG_WATER_REMOTION_RATE);
		water = true;
	}
			
	++(this->MOVE_IT);
	return MillingResult(this->stepNumber, infos, water);
}

bool MillingAlgorithm::hasFinished() {
	return this->MOVE_IT == this->MOVE_END;
}

u_int MillingAlgorithm::getStepNumber() {
	return this->stepNumber;
}

Eigen::Vector3d MillingAlgorithm::getResolution() const {
	return this->STOCK->getResolution();
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
	
	return STOCK->intersect(CUTTER, cutterIsom_stock);
}

std::ostream& operator <<(std::ostream& os, const MillingAlgorithm& ma) {
	os << "MILLING_ALGORITHM(currStep#:" << ma.stepNumber << "; next_move:";
	if (ma.MOVE_IT == ma.MOVE_END)
		os << "ENDED";
	else
		os << *(ma.MOVE_IT);
	os << ")" << std::endl
			<< "\tCutter: " << *ma.CUTTER //; ma.cutter->toOutStream(os)
			<< std::endl
			<< "\tStock: " << *ma.STOCK;
	os << std::endl << "END_MILLING_ALGORITHM";
	
	return os;
}
