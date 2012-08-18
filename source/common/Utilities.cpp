/*
 * Utilities.cpp
 *
 *  Created on: 17/ago/2012
 *      Author: socket
 */

#include <iostream>
#include <fstream>

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

readData FileUtils::readNextValidLine(std::istream& ifs) throw(std::runtime_error) {
	std::string str;
	std::streampos lastReadPos;
	
	do {
		if (!ifs.good())
			throw std::runtime_error("unexpected EOF");
		
		lastReadPos = ifs.tellg();
		std::getline(ifs, str);
	} while (isSkippableLine(str));
	
	readData data;
	data.validLine = str;
	data.lastReadPos = lastReadPos;
	
	return data;
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

