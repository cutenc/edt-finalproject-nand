/*
 * Stock.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef STOCK_HPP_
#define STOCK_HPP_

#include <Eigen/Geometry>

#include "cutters.hpp"
#include "MeshingInfo.hpp"
#include "Octree.hpp"
#include "configuration/StockDescription.hpp"

struct IntersectionResult {
	
	/**
	 * Gives an estimation of how much volume has been removed during the
	 * milling operation
	 */
	double waste;
	
	/**
	 * Tells wether cutter were totally out of the stock (no intersection) or
	 * not.
	 */
	bool outOfStock;
};

class Stock {
	
//	const Octree< MeshingInfo > MODEL;
	
public:
	Stock(const StockDescription &desc);
	virtual ~Stock();
	
	IntersectionResult intersect(CutterPtr cutter, Eigen::Vector3d traslation, Eigen::Matrix3d rotation);
	
};


#endif /* STOCK_HPP_ */
