/*
 * ConfigurationManager.cpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#include "ConfigurationManager.hpp"

#include <iostream>
#include <fstream>
#include <string>

ConfigurationManager::ConfigurationManager(std::string filename,
		int nLinesBuffer) {
	
	std::ifstream ifs(filename.c_str(), std::ifstream::in);
	
	while(ifs.good() && !foundAll()) {
		
	}
	ifs.close();
	
}

ConfigurationManager::~ConfigurationManager() {
	// TODO Auto-generated destructor stub
}

const StockDescription& ConfigurationManager::getStockDescription() const {
	return stock;
}

const CutterDescription& ConfigurationManager::getCutterDescription() const {
	return cutter;
}

CNCMoveIterator ConfigurationManager::getCNCMoveIterator() const {
}

bool ConfigurationManager::foundAll() const {
	return this->foundCutter && this->foundStock && this->foundPoints;
}

