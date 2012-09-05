/*
 * Utilities.cpp
 *
 *  Created on: 17/ago/2012
 *      Author: socket
 */

#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include "Utilities.hpp"
#include "constants.hpp"

void FileUtils::openFile(const std::string &filename, std::ifstream &ifs) {
	
	/* don't add also failbit because some opertions set failbit when eof is
	 * reached
	 */
	ifs.exceptions(std::ios_base::badbit);
	ifs.open(filename.c_str(), std::ifstream::in);
	
	if (!ifs.is_open()) {
		throw std::ios_base::failure("can't open given file: " + filename);
	}
	
}

FileUtils::ReadData FileUtils::readNextValidLine(std::istream& ifs) throw(std::runtime_error) {
	std::string str;
	std::streampos lastReadPos;
	
	lastReadPos = ifs.tellg();
	
	while(std::getline(ifs, str)) {
		if (!isSkippableLine(str)) {
			return FileUtils::ReadData(str, lastReadPos);
		} else {
			lastReadPos = ifs.tellg();
		}
		
	}
	
	throw std::runtime_error("EOF reached");
}

bool FileUtils::isSkippableLine(std::string &line) {
	boost::trim(line);
	
	// check if the trimmed string is empty
	if (line.empty())
		return true;
	
	// check if it is a comment
	if (line[0] == CONF_COMMENT_CHAR)
		return true;
	
	return false;
}

std::string StringUtils::extractProperty(const std::string &line,
		const std::string &propName, const std::string &propValuePattern, bool icase) 
		throw(std::runtime_error){
	
	std::string strExp(propName + "\\s*=\\s*(" + propValuePattern + ")");
	
	boost::smatch match;
	boost::regex regex(strExp, ((icase) ? boost::regex::icase : boost::regex::normal));
	
	if (!boost::regex_match(line, match, regex)) {
		throw std::runtime_error("No match found in '" + line + "' for '" + strExp + "'");
	}
	
	return match[1];
}

std::string StringUtils::repeat(const std::string &pattern, u_int nTimes) {
	std::stringstream sstream;
	for (u_int i = 0; i < nTimes; ++i) {
		sstream << pattern;
	}
	
	return sstream.str();
}

const double CommonUtils::INFINITE = (std::numeric_limits<double>::has_infinity) ? 
		std::numeric_limits<double>::infinity() : std::numeric_limits<double>::max();

