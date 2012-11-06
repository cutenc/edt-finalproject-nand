/*
 * CommandLineParser.cpp
 *
 *  Created on: 20/ago/2012
 *      Author: socket
 */

#include "CommandLineParser.hpp"

#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

CommandLineParser::CommandLineParser(int argc, char** argv) : 
		OPTIONS(buildOptions()), POSITIONALS(buildPositionals()), 
		PROG_NAME(argv[0]) {
	
	bpo::variables_map vm;
	bpo::store(bpo::command_line_parser(argc, argv).
			options(OPTIONS).
			positional(POSITIONALS).run(),
			vm);
	bpo::notify(vm);
	
	this->helpAsked = vm.count("help");
	this->paused = vm.count("paused");
}

CommandLineParser::~CommandLineParser() {
}


std::string CommandLineParser::getConfigFile() const {
	return this->filename;
}

float CommandLineParser::getMinVoxelSize() const {
	return this->minVoxelSize;
}

bool CommandLineParser::isHelpAsked() const {
	return this->helpAsked;
}

CommandLineParser::VideoMode CommandLineParser::getVideoMode() const {
	return this->videoMode;
}

bool CommandLineParser::startPaused() const {
	return this->paused;
}

float CommandLineParser::getWaterFlux() const {
	return this->waterFlux;
}

float CommandLineParser::getWaterThreshold() const {
	return this->waterThreshold;
}

void CommandLineParser::printUsage(std::ostream& os) const {
	os << "Usage: " << PROG_NAME << " [options] pointsFile" << std::endl;
	os << OPTIONS << std::endl;
}

