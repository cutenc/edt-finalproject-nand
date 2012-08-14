/*
 * ConfigurationManager.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef CONFIGURATIONMANAGER_HPP_
#define CONFIGURATIONMANAGER_HPP_

#include <string>

#include "StockDescription.hpp"
#include "CutterDescription.hpp"
#include "CNCMoveIterator.hpp"

class ConfigurationManager {
public:
	ConfigurationManager(std::string filename, int nLinesBuffer);
	virtual ~ConfigurationManager();
	
	const StockDescription &getStockDescription() const;
	const CutterDescription &getCutterDescription() const;
	CNCMoveIterator getCNCMoveIterator() const;
	
private:
	bool foundAll() const;
	
	StockDescription stock;
	CutterDescription cutter;
	bool foundPoints, foundCutter, foundStock;
};

#endif /* CONFIGURATIONMANAGER_HPP_ */
