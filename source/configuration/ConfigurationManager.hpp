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

class ConfigurationManager;

typedef void (ConfigurationManager::*sectionParser)(std::ifstream &);
typedef std::map< std::string, sectionParser > ParsersMap;

typedef struct _readData {
	std::string validLine;
	std::streamoff dataRead;
	std::streamoff lastDataRead;
} readData;

class ConfigurationManager {
public:
	ConfigurationManager(const std::string filename, const int nLinesBuffer);
	virtual ~ConfigurationManager();
	
	const StockDescriptionPtr getStockDescription() const;
	const CutterDescriptionPtr getCutterDescription() const;
	CNCMoveIterator getCNCMoveIterator() const;
	
private:
	bool foundAll() const;
	
	/**
	 * Checks if given string can be skipped during the parsing of the
	 * configuration file. Line will be trimmed inside the method.
	 * @param line
	 * @return
	 */
	bool isSkippableLine(std::string &line) const;
	
	/**
	 * Read and return a valid line from given std::ifstream
	 * @param 
	 * @return
	 * 
	 * @throw std::runtime_error if there's no other valid lines in the given
	 * stream.
	 */
	readData readNextValidLine(std::ifstream &) const throw(std::runtime_error);
	
	/**
	 * Returns the result of the match of given \c line against the reg.exp.
	 * pattern formed as follow: 
	 * @code
	 * 	propName + "\\s*=\\s*(" + propValuePatter + ")"
	 * @endcode
	 * 
	 * @param line
	 * @param propName
	 * @param propValuePattern
	 * @param icase when enabled (default) perform a case-insensitive match.
	 * @return
	 * 
	 * @throw std::runtime_error when given line does not match.
	 */
	std::string extractProperty(const std::string &line, const std::string &propName, 
			const std::string &propValuePattern, bool icase = true) const throw(std::runtime_error);
	
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
		m["TOOL"] = &ConfigurationManager::sectionParser_tool;
		m["PRODUCT"] = &ConfigurationManager::sectionParser_product;
		m["POINTS"] = &ConfigurationManager::sectionParser_points;
		return m;
	}
	
	const std::string FILENAME;
	const int N_LINE_BUFFER;
	const ParsersMap PARSERS;
	StockDescriptionPtr stock;
	CutterDescriptionPtr cutter;
	bool foundPoints, foundCutter, foundStock;
};

#endif /* CONFIGURATIONMANAGER_HPP_ */
