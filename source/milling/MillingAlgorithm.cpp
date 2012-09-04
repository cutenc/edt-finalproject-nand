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

MillingAlgorithm::MillingAlgorithm(const ConfigFileParser &cfp, u_int maxOctreeDepth) :
		MAX_OCTREE_DEPTH(maxOctreeDepth),
		MOVE_IT(cfp.CNCMoveBegin()), MOVE_END(cfp.CNCMoveEnd()),
		waterFluxWasteCount(0.0), stepNumber(0) {
	
	stock = boost::make_shared< Stock >(*cfp.getStockDescription(), MAX_OCTREE_DEPTH);
	cutter = Cutter::buildCutter(*cfp.getCutterDescription());
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
	return this->stock->getResolution();
}

IntersectionResult MillingAlgorithm::doIntersection(const CNCMove &move) {
	
	/* given move rototraslations are in respect of world basis so we have
	 * to "merge" this two informations in order to find cutter rototraslation
	 * in stock basis
	 */
	
	Point3D stockTrasl_world = move.STOCK.TRASLATION,
			cutterTrasl_world = move.CUTTER.TRASLATION;
	
	Eigen::Vector3d cutterTrasl_stock = cutterTrasl_world.asEigen() -
			stockTrasl_world.asEigen();
	
	EulerAngles stockRot_world = move.STOCK.ROTATION,
			cutterRot_world = move.CUTTER.ROTATION;
	
	/* when returned as eigen both these rotations are matrices that
	 * converts point from stock/cutter coords to world ones: i.e. they
	 * convert from stock/cutter basis to world basis; we have to create 
	 * a rotation matrix that converts from cutter coords to stock coords.
	 * NB: because all rotation matrix are orthonormal we know that
	 * M^(-1) = M^T (inverse equals transposed)
	 */
	Eigen::Matrix3d cutterRot_stock = stockRot_world.asEigen().transpose() * 
			cutterRot_world.asEigen();
	
	return stock->intersect(cutter, cutterTrasl_stock, cutterRot_stock);
}

std::ostream& operator <<(std::ostream& os, const MillingAlgorithm& ma) {
	os << "MILLING_ALGORITHM(currStep#:" << ma.stepNumber << "; next_move:";
	if (ma.MOVE_IT == ma.MOVE_END)
		os << "ENDED";
	else
		os << *(ma.MOVE_IT);
	os << ")" << std::endl
			<< "\tCutter: " << *ma.cutter //; ma.cutter->toOutStream(os)
			<< std::endl
			<< "\tStock: " << *ma.stock;
	os << std::endl << "END_MILLING_ALGORITHM";
	
	return os;
}
