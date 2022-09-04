/**
 * @file main.cpp
 *
 * main file of the project
 */

#include <iostream>
#include <cmath>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>

#include "configuration/ConfigFileParser.hpp"
#include "configuration/CommandLineParser.hpp"
#include "milling/MillingAlgorithm.hpp"
#include "milling/Stock.hpp"
#include "milling/cutters.hpp"
#include "meshing/VoxelMesher.hpp"
#include "meshing/MarchingCubeMesher.hpp"
#include "meshing/StubMesher.hpp"
#include "visualizer/MillerRunnable.hpp"
#include "visualizer/Display.hpp"
#include "visualizer/DisplayTextual.hpp"
#include "visualizer/KeyboardManager.hpp"

//#define LOG_2 0.69314718055994530941723212145817656807550013436025


using namespace std;
using namespace Eigen;

/**
 * main
 *
 * @param argc : number of command-line parameters
 * @param argv : the command-line parameters
 * @return exit status of the program
 */
int main(int argc, char **argv) {

	cout 	<< "****************************************" << endl
			<< "************* CNCSimulator *************" << endl
			<< "**** Nicola Gobbo & Alberto Franzin ****" << endl
			<< "********************************* v0.1 *" << endl;

	CommandLineParser clp(argc, argv);
	
	if (clp.isHelpAsked()) {
		clp.printUsage(cout);
		return 0;
	}
	
	std::string configFile = clp.getConfigFile();
	
	ConfigFileParser cfp(configFile);
	
	// **** BUILD STOCK **** //
	// calculate max octree depth
	double maxDim = cfp.getStockDescription()->getGeometry()->asEigen().maxCoeff();
	float minSize = clp.getMinVoxelSize();
	unsigned int max_depth = log(maxDim / minSize) / LOG_2 + 1;
	
	Mesher< StoredData >::Ptr mesher;
	switch (clp.getVideoMode()) {
		case CommandLineParser::NONE:
			mesher = boost::make_shared< StubMesher< StoredData > >();
			break;
			
		case CommandLineParser::MESH:
			mesher = boost::make_shared< MarchingCubeMesher >(*cfp.getStockDescription());
			break;
			
		case CommandLineParser::BOX:
			mesher = boost::make_shared< VoxelMesher >(*cfp.getStockDescription());
			break;
			
		default:
			throw std::runtime_error("Unknonw video mode");
	}
	Stock::Ptr stock = boost::make_shared< Stock >(*cfp.getStockDescription(), max_depth, mesher);
	
	// **** BUILD CUTTER **** //
	Cutter::Ptr cutter = Cutter::buildCutter(*cfp.getCutterDescription());
	
	// **** BUILD MILLING ALGORITHM **** //
	MillingAlgorithmConf millingConf(stock, cutter, cfp.CNCMoveBegin(), cfp.CNCMoveEnd(),
			clp.getWaterFlux(), clp.getWaterThreshold());
	MillingAlgorithm::Ptr algorithm = boost::make_shared< MillingAlgorithm >(millingConf);
	
	// **** BUILD MILLER RUNNABLE **** //
	MillingSignaler::Ptr signaler = boost::make_shared< MillingSignaler >();
	SteppableController::Ptr controller = boost::make_shared< SteppableController >(clp.startPaused());
	MillerRunnable miller(controller, signaler, algorithm);
	
	cout << "Setup info: " << endl
			<< "\tPosition file: " << configFile << endl
			<< "\tCutter: " << *cutter << endl
			<< "\tStock: " << *stock << endl
			;
	
	// **** LAUNCH MILLER & DISPLAYER **** //
	boost::thread millerThrd(boost::ref(miller));
	
	switch (clp.getVideoMode()) {
		case CommandLineParser::NONE: {
			// set up a textual visualization of what is being milled
			InputDeviceStateType::Ptr idst = boost::make_shared< InputDeviceStateType >();
			DisplayTextual display(idst, signaler);
			boost::thread displayThrd(boost::ref(display));
			
			if (clp.startPaused()) {
				// then we have to create a manager for pressed keys
				
				// create keyboard manager
				KeyboardManager km(idst, controller);
				
				char readC;
				do {
					cin >> readC;
					km.process(readC);
				} while (readC != 27); // that is, ESC key
				controller->stop();
			}
			
			displayThrd.join();
			break;
		}
		
		case CommandLineParser::MESH:
		case CommandLineParser::BOX: {
			Display display(stock, cutter, signaler, controller);
			display.draw();
			break;
		}
		
		default:
			throw std::runtime_error("Unknown video mode");
	}

	controller->stop(); // ... just in case someone forgot to call it ...

	millerThrd.join();

	return 0;
}




