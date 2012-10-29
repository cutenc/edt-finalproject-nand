/*
 * MillingAlgorithmConf.hpp
 *
 *  Created on: 23/set/2012
 *      Author: socket
 */

#ifndef MILLINGALGORITHMCONF_HPP_
#define MILLINGALGORITHMCONF_HPP_

#include "Stock.hpp"
#include "Cutter.hpp"
#include "configuration/CNCMoveIterator.hpp"

class MillingAlgorithmConf {
public:
	MillingAlgorithmConf(Stock::Ptr stock, Cutter::ConstPtr cutter,
			const CNCMoveIterator &begin, const CNCMoveIterator &end,
			float waterRemotionRate, float waterThreshold) :
				STOCK(stock), CUTTER(cutter), MOVE_IT(begin), MOVE_END(end),
				waterFlux(waterRemotionRate), waterThreshold(waterThreshold)
	{
		
	}
				
	virtual ~MillingAlgorithmConf() { }
	
	const Stock::Ptr STOCK;
	const Cutter::ConstPtr CUTTER;
	CNCMoveIterator MOVE_IT, MOVE_END;
	const float waterFlux;
	const float waterThreshold;
};

#endif /* MILLINGALGORITHMCONF_HPP_ */
