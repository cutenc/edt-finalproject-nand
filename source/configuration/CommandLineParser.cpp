/*
 * CommandLineParser.cpp
 *
 *  Created on: 20/ago/2012
 *      Author: socket
 */

#include "CommandLineParser.hpp"

#include <boost/program_options.hpp>

namespace bpo = boost::program_options;

CommandLineParser::CommandLineParser(int argc, const char** argv) : 
		OPTIONS(buildOptions()), POSITIONALS(buildPositionals()), 
		PROG_NAME(argv[0]) {
	
	bpo::variables_map vm;
	bpo::store(bpo::command_line_parser(argc, argv).
			options(OPTIONS).
			positional(POSITIONALS).run(),
			vm);
	bpo::notify(vm);
	
	this->helpAsked = vm.count("help");
	this->verbosity = vm.count("verbose");
}

CommandLineParser::~CommandLineParser() {
}


std::string CommandLineParser::getConfigFile() const {
	return this->filename;
}

int CommandLineParser::getMaxOctreeHeight() const {
	return this->octreeHeight;
}

bool CommandLineParser::isHelpAsked() const {
	return this->helpAsked;
}

u_char CommandLineParser::verbosityLevel() const {
	return this->verbosity;
}

void CommandLineParser::printUsage(std::ostream& os) const {
	os << "Usage: " << PROG_NAME << " [options] pointsFile" << std::endl;
	os << OPTIONS << std::endl;
}

