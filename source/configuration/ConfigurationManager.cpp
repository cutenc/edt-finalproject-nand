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
#include <stdexcept>
#include <cassert>

#include <boost/regex.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>

#include "common/constants.hpp"

ConfigurationManager::ConfigurationManager(const std::string filename,
		const int nLinesBuffer) : FILENAME(filename), N_LINE_BUFFER(nLinesBuffer),
		PARSERS(fillParsers()) {
	
	foundCutter = foundStock = foundPoints = false;
	
	std::ifstream ifs(filename.c_str(), std::ifstream::in);
	
	if (!ifs.is_open()) {
		throw std::ios_base::failure("can't open given file");
	}
	
	while(ifs.good() && !foundAll()) {
		std::string str = readNextValidLine(ifs).validLine;
		
		// checks if given line is a section string of type: [SMTHNG]
		boost::smatch matcher;
		boost::regex expression("\\[(\\w+)\\]");
		if (!boost::regex_match(str, matcher, expression)) {
			abortParsing("'" + str + "' is not a valid section");
		}
		std::string section(matcher[1]);
		
		ParsersMap::const_iterator it;
		it = PARSERS.find(section);
		if (it == PARSERS.end()) {
			// no parsers registered to dissect given section
			abortParsing("unknown section '" + section + "'");
		}
		
		// invoke parser in order to dissect section
		(this->*(it->second))(ifs);
		
		// then continue to the next section
	}
	
	if (!foundAll()) {
		std::string errorStr("missing section[s]: ");
		if (!foundCutter)
			errorStr += "tool ";
		if (!foundPoints)
			errorStr += "point ";
		if (!foundStock)
			errorStr += "product ";
		abortParsing(errorStr);
	}
}

ConfigurationManager::~ConfigurationManager() { }

const StockDescriptionPtr ConfigurationManager::getStockDescription() const {
	return stock;
}

const CutterDescriptionPtr ConfigurationManager::getCutterDescription() const {
	return cutter;
}

CNCMoveIterator ConfigurationManager::getCNCMoveIterator() const {
}

bool ConfigurationManager::foundAll() const {
	return this->foundCutter && this->foundStock && this->foundPoints;
}

readData ConfigurationManager::readNextValidLine(std::ifstream& ifs) const throw(std::runtime_error) {
	std::string str;
	std::streampos initPos = ifs.tellg();
	std::streampos lastReadPos;
	
	do {
		if (!ifs.good())
			throw std::runtime_error("unexpected EOF");
		
		lastReadPos = ifs.tellg();
		std::getline(ifs, str);
	} while (isSkippableLine(str));
	
	std::streampos actualPos = ifs.tellg();
	
	readData data;
	data.validLine = str;
	data.dataRead = actualPos - initPos;
	data.lastDataRead = actualPos - lastReadPos;
	
	return data;
}

bool ConfigurationManager::isSkippableLine(std::string &line) const {
	boost::trim(line);
	
	// check if the trimmed string is empty
	if (line.empty())
		return true;
	
	// check if it is a comment
	if (line[0] == CONF_COMMENT_CHAR)
		return true;
	
	return false;
}

void ConfigurationManager::abortParsing(const std::string &cause) const 
		throw(std::runtime_error) {
	
	std::string str("[" + FILENAME + "] malformed configuration file: " + cause);
	throw std::runtime_error(str);
}

std::string ConfigurationManager::extractProperty(const std::string &line,
		const std::string &propName, const std::string &propValuePattern, bool icase) const 
		throw(std::runtime_error){
	
	std::string strExp(propName + "\\s*=\\s*(" + propValuePattern + ")");
	
	boost::smatch match;
	boost::regex regex(strExp, ((icase) ? boost::regex::icase : boost::regex::normal));
	
	if (!boost::regex_match(line, match, regex)) {
		throw std::runtime_error("No match found in '" + line + "' for '" + strExp + "'");
	}
	
	return match[1];
}

void ConfigurationManager::sectionParser_tool(std::ifstream& ifs) {
	if (foundCutter)
		abortParsing("Repeated TOOL section");
	
	std::string line = readNextValidLine(ifs).validLine;
	
	// 1- discover type
	boost::smatch match;
	boost::regex exp("type\\s*=\\s*(.+)", boost::regex::icase);
	
	if(!boost::regex_match(line, match, exp)) {
		abortParsing("first line in tool definition should be 'Type=tool_type'");
	}
	std::string type(match[1]);
	boost::algorithm::to_lower(type);
	
	// 2- extract correct informations based on type
	GeometryPtr geometry;
	if (type == "cylinder") {
		// next line should be 'Height=NN':
		line = readNextValidLine(ifs).validLine;
		std::string heightStr = extractProperty(line, "Height", "[\\d\\.]+", true);
		// next line should be 'Diameter=NN':
		line = readNextValidLine(ifs).validLine;
		std::string diameterStr = extractProperty(line, "Diameter", "[\\d\\.]+", true);
		
		double height = boost::lexical_cast<double>(heightStr);
		double diameter = boost::lexical_cast<double>(diameterStr);
		
		geometry = boost::make_shared<Cylinder>(Cylinder(diameter / 2.0, height));
		
	} else if (type == "sphere") {
		// next line should be 'Diameter=NN':
		line = readNextValidLine(ifs).validLine;
		std::string diameterStr = extractProperty(line, "Diameter", "[\\d\\.]+", true);
		
		double diameter = boost::lexical_cast<double>(diameterStr);
		
		geometry = boost::make_shared<Sphere>(Sphere(diameter / 2.0));
		
	} else if (type == "mesh") {
		// TODO need to be implemented
		throw std::runtime_error("Not implemented yet");
		
	}
	
	// 3- find an _optional_ color specification
	Color color;
	readData data = readNextValidLine(ifs);
	try {
		std::string colorStr = extractProperty(data.validLine, "color", "0x[\\da-f]{6}", true);
		
		color = Color(colorStr);
		
	} catch (const std::exception &e) {
		// means no color is present => revert ifs back to the previous line
		ifs.seekg(-data.lastDataRead, std::ios_base::cur);
	}
	
	this->cutter = boost::make_shared<CutterDescription>(CutterDescription(geometry, color));
	this->foundCutter = true;
}

void ConfigurationManager::sectionParser_product(std::ifstream& ifs) {
	if(foundStock)
		abortParsing("Repeated PRODUCT section");
	
	// 1- X=NN
	std::string line = readNextValidLine(ifs).validLine;
	std::string XStr = extractProperty(line, "X", "[\\d\\.]+", true);
	double X = boost::lexical_cast<double>(XStr);
	
	// 2- Y=NN
	line = readNextValidLine(ifs).validLine;
	std::string YStr = extractProperty(line, "Y", "[\\d\\.]+", true);
	double Y = boost::lexical_cast<double>(YStr);
	
	// 3- Z=NN
	line = readNextValidLine(ifs).validLine;
	std::string ZStr = extractProperty(line, "Z", "[\\d\\.]+", true);
	double Z = boost::lexical_cast<double>(ZStr);
	
	RectCuboidPtr geom = boost::make_shared<RectCuboid>(RectCuboid(X, Y, Z));
	
	this->stock = boost::make_shared<StockDescription>(StockDescription(geom));
	this->foundStock = true;
}

void ConfigurationManager::sectionParser_points(std::ifstream& ifs) {
	if (foundPoints)
			abortParsing("Repeated POINTS section");
	
	ifs.good(); // so GCC does not complain about unused variables
	
	this->foundPoints = true;
	
	if (!foundAll()) {
		/* this means that points are not placed at the end of the file so
		 * i would have to scan all points until the end of the file to check
		 * if there are some other sections there.......
		 */
		throw std::runtime_error("missing sections or POINTS is not last section");
	}
}




