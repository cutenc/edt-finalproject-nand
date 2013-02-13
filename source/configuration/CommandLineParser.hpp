/**
 * @file CommandLineParser.hpp
 *
 *  Created on: 20/ago/2012
 *      Author: socket
 */

#ifndef COMMANDLINEPARSER_HPP_
#define COMMANDLINEPARSER_HPP_

#include <string>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "common/constants.hpp"

namespace bpo = boost::program_options;

/**
 * @class CommandLineParser
 *
 * reads and interprets the parameters passed in the command line.
 */
class CommandLineParser {
	
public:
	/**
	 * display modes
	 */
	enum VideoMode {
		NONE,//!< NONE
		BOX, //!< BOX
		MESH //!< MESH
	};

	/**
	 * overload istream>> operator in order to accept VideoModes
	 * @param is the input stream
	 * @param vm the VideoMode
	 * @return
	 */
	friend std::istream &operator>>(std::istream &is, CommandLineParser::VideoMode &vm) {
		std::string token;
		is >> token;
		
		if (boost::iequals(token, "none")) {
			vm = CommandLineParser::NONE;
		} else if (boost::iequals(token, "box")) {
			vm = CommandLineParser::BOX;
		} else if (boost::iequals(token, "mesh")) {
			vm = CommandLineParser::MESH;
		} else {
			throw std::runtime_error("Invalid video type '" + token + "'");
		}
		
		return is;
	}
	
private:
	const bpo::options_description OPTIONS;
	const bpo::positional_options_description POSITIONALS;
	const std::string PROG_NAME;
	
	std::string filename;
	VideoMode videoMode;
	float minVoxelSize;
	float waterFlux;
	float waterThreshold;
	bool helpAsked;
	bool paused;
	
public:

	/**
	 * constructor
	 * @param argc
	 * @param argv
	 */
	CommandLineParser(int argc, char **argv);

	/**
	 * destructor
	 */
	virtual ~CommandLineParser();

	/**
	 *
	 * @return the path/name of the config file
	 */
	std::string getConfigFile() const;

	/**
	 *
	 * @return the minimum size of the voxel
	 */
	float getMinVoxelSize() const;

	/**
	 *
	 * @return the water removal rate
	 */
	float getWaterFlux() const;

	/**
	 *
	 * @return the water removal threshold
	 */
	float getWaterThreshold() const;

	/**
	 *
	 * @return True if help is asked, False otherwise
	 */
	bool isHelpAsked() const;

	/**
	 *
	 * @return True if the simulator has to be started paused
	 */
	bool startPaused() const;

	/**
	 *
	 * @return the chosen video mode
	 */
	VideoMode getVideoMode() const;

	/**
	 * print the helper
	 * @param os
	 */
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
				("vsize,s", bpo::value< float >(&minVoxelSize)->default_value(CMDLN_MIN_VOXEL_SIZE), "minimum voxel size: all voxel dimensions should be equal or less then specified value")
				("video,v", bpo::value< VideoMode >(&videoMode)->default_value(CMDLN_VIDEO_MODE), "set video mode: 'box', 'mesh', 'none' (default)")
				("paused,p", "starts program in paused mode, you'll need to press RUN to start milling")
				("wflux,f", bpo::value< float >(&waterFlux)->default_value(ALG_WATER_REMOTION_RATE), "set water removal rate (in u^3 of waste)")
				("wthreshold,t", bpo::value< float >(&waterThreshold)->default_value(ALG_WATER_THRESHOLD), "set amount of waste to mill before enabling water (in u^3)")
		;
		
		return description;
	}
};

#endif /* COMMANDLINEPARSER_HPP_ */
