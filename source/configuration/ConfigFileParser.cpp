/*
 * ConfigurationManager.cpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#include "ConfigFileParser.hpp"

#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <boost/regex.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "common/Utilities.hpp" 

ConfigFileParser::ConfigFileParser(const std::string filename) : 
		FILENAME(filename), PARSERS(fillParsers()) {
	
	foundCutter = foundStock = foundPoints = false;
	
	std::ifstream ifs;
	FileUtils::openFile(filename, ifs);
	
	while(ifs.good() && !foundAll()) {
		std::string str = FileUtils::readNextValidLine(ifs).validLine;
		
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

ConfigFileParser::~ConfigFileParser() { }

const StockDescriptionPtr ConfigFileParser::getStockDescription() const {
	return stock;
}

const CutterDescriptionPtr ConfigFileParser::getCutterDescription() const {
	return cutter;
}

CNCMoveIterator ConfigFileParser::CNCMoveBegin() const {
	boost::shared_ptr< std::ifstream > ifsp = boost::make_shared< std::ifstream >();
	FileUtils::openFile(this->FILENAME, *ifsp);
	
	if (!ifsp->is_open()) {
		throw std::runtime_error("File " + this->FILENAME + " is disappeared");
	}
	
	ifsp->seekg(this->firstPointPos);
	
	return CNCMoveIterator(ifsp);
}

CNCMoveIterator ConfigFileParser::CNCMoveEnd() const {
	return CNCMoveIterator();
}

bool ConfigFileParser::foundAll() const {
	return this->foundCutter && this->foundStock && this->foundPoints;
}


void ConfigFileParser::abortParsing(const std::string &cause) const 
		throw(std::runtime_error) {
	
	std::string str("[" + FILENAME + "] malformed configuration file: " + cause);
	throw std::runtime_error(str);
}

void ConfigFileParser::sectionParser_tool(std::ifstream& ifs) {
	if (foundCutter)
		abortParsing("Repeated TOOL section");
	
	std::string line = FileUtils::readNextValidLine(ifs).validLine;
	
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
		line = FileUtils::readNextValidLine(ifs).validLine;
		std::string heightStr = StringUtils::extractProperty(line, "Height", "[\\d\\.]+", true);
		// next line should be 'Diameter=NN':
		line = FileUtils::readNextValidLine(ifs).validLine;
		std::string diameterStr = StringUtils::extractProperty(line, "Diameter", "[\\d\\.]+", true);
		
		double height = boost::lexical_cast<double>(heightStr);
		double diameter = boost::lexical_cast<double>(diameterStr);
		
		geometry = boost::make_shared<Cylinder>(Cylinder(diameter / 2.0, height));
		
	} else if (type == "sphere") {
		// next line should be 'Diameter=NN':
		line = FileUtils::readNextValidLine(ifs).validLine;
		std::string diameterStr = StringUtils::extractProperty(line, "Diameter", "[\\d\\.]+", true);
		
		double diameter = boost::lexical_cast<double>(diameterStr);
		
		geometry = boost::make_shared<Sphere>(Sphere(diameter / 2.0));
		
	} else if (type == "mesh") {
		// TODO need to be implemented
		throw std::runtime_error("Not implemented yet");
		
	}
	
	// 3- find an _optional_ color specification
	Color color;
	FileUtils::ReadData data = FileUtils::readNextValidLine(ifs);
	try {
		std::string colorStr = StringUtils::extractProperty(data.validLine, "color", "0x[\\da-f]{6}", true);
		
		color = Color(colorStr);
		
	} catch (const std::exception &e) {
		// means no color is present => revert ifs back to the previous line
		ifs.seekg(data.lastReadPos);
	}
	
	this->cutter = boost::make_shared<CutterDescription>(CutterDescription(geometry, color));
	this->foundCutter = true;
}

void ConfigFileParser::sectionParser_product(std::ifstream& ifs) {
	if(foundStock)
		abortParsing("Repeated PRODUCT section");
	
	// 1- X=NN
	std::string line = FileUtils::readNextValidLine(ifs).validLine;
	std::string XStr = StringUtils::extractProperty(line, "X", "[\\d\\.]+", true);
	double X = boost::lexical_cast<double>(XStr);
	
	// 2- Y=NN
	line = FileUtils::readNextValidLine(ifs).validLine;
	std::string YStr = StringUtils::extractProperty(line, "Y", "[\\d\\.]+", true);
	double Y = boost::lexical_cast<double>(YStr);
	
	// 3- Z=NN
	line = FileUtils::readNextValidLine(ifs).validLine;
	std::string ZStr = StringUtils::extractProperty(line, "Z", "[\\d\\.]+", true);
	double Z = boost::lexical_cast<double>(ZStr);
	
	RectCuboidPtr geom = boost::make_shared<RectCuboid>(RectCuboid(X, Y, Z));
	
	this->stock = boost::make_shared<StockDescription>(StockDescription(geom));
	this->foundStock = true;
}

void ConfigFileParser::sectionParser_points(std::ifstream& ifs) {
	if (foundPoints)
		abortParsing("Repeated POINTS section");
	
	this->firstPointPos = ifs.tellg();
	this->foundPoints = true;
	
	if (!foundAll()) {
		/* this means that points are not placed at the end of the file so
		 * i would have to scan all points until the end of the file to check
		 * if there are some other sections there.......
		 */
		throw std::runtime_error("missing sections or POINTS is not last section");
	}
}




