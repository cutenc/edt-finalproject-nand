/*
 * ConfigurationManager.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef CONFIGURATIONMANAGER_HPP_
#define CONFIGURATIONMANAGER_HPP_

#include <string>
#include <map>

#include "StockDescription.hpp"
#include "CutterDescription.hpp"
#include "CNCMoveIterator.hpp"

class ConfigFileParser;

typedef void (ConfigFileParser::*sectionParser)(std::ifstream &);
typedef std::map< std::string, sectionParser > ParsersMap;


class ConfigFileParser {
	
	const std::string FILENAME;
	const ParsersMap PARSERS;
	StockDescriptionPtr stock;
	CutterDescriptionPtr cutter;
	std::streampos firstPointPos;
	bool foundPoints, foundCutter, foundStock;
	
public:
	ConfigFileParser(const std::string filename);
	virtual ~ConfigFileParser();
	
	const StockDescriptionPtr getStockDescription() const;
	const CutterDescriptionPtr getCutterDescription() const;
	CNCMoveIterator CNCMoveBegin() const;
	CNCMoveIterator CNCMoveEnd() const;
	
private:
	bool foundAll() const;
	
	/**
	 * Simply throws an exception with a standard-formatted message
	 * 
	 * @param cause
	 */
	void abortParsing(const std::string &cause) const throw(std::runtime_error);
	
	void sectionParser_tool(std::ifstream &);
	void sectionParser_product(std::ifstream &);
	void sectionParser_points(std::ifstream &);
	
	ParsersMap fillParsers() {
		std::map< std::string, sectionParser > m;
		m["TOOL"] = &ConfigFileParser::sectionParser_tool;
		m["PRODUCT"] = &ConfigFileParser::sectionParser_product;
		m["POINTS"] = &ConfigFileParser::sectionParser_points;
		return m;
	}
	
};

#endif /* CONFIGURATIONMANAGER_HPP_ */
