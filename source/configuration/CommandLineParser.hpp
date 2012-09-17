/*
 * CommandLineParser.hpp
 *
 *  Created on: 20/ago/2012
 *      Author: socket
 */

#ifndef COMMANDLINEPARSER_HPP_
#define COMMANDLINEPARSER_HPP_

#include <string>
#include <iostream>

#include <boost/program_options.hpp>

#include "common/constants.hpp"

namespace bpo = boost::program_options;

class CommandLineParser {
	
	const bpo::options_description OPTIONS;
	const bpo::positional_options_description POSITIONALS;
	const std::string PROG_NAME;
	
	std::string filename;
	int octreeHeight;
	bool helpAsked;
	u_char verbosity;
	
public:
	CommandLineParser(int argc, const char **argv);
	virtual ~CommandLineParser();
	
	std::string getConfigFile() const;
	int getMaxOctreeHeight() const;
	bool isHelpAsked() const;
	u_char verbosityLevel() const;
	void printUsage(std::ostream &os) const;
	
private:
	
	bpo::positional_options_description buildPositionals() {
		bpo::positional_options_description positionals;
		positionals.add("config", -1);
		
		return positionals;
	}
	
	bpo::options_description buildOptions() {
		bpo::options_description description("Available options");
		description.add_options()
				("help,h", "produces this help message")
				("config,c", bpo::value< std::string >(&filename)->default_value(CMDLN_CONFFILE_NAME), "position of the configuration file")
				("max-height,o", bpo::value< int >(&octreeHeight)->default_value(CMDLN_MAX_OCTREE_HEIGHT), "max octree height: this also defines minimum voxel size")
				("verbose,v", "the output will be verbose")
		;
		
		return description;
	}
};

#endif /* COMMANDLINEPARSER_HPP_ */
