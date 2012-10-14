/*
 * Visualizer.cpp
 *
 *  Created on: 18/set/2012
 *      Author: alberto
 */

#include "visualizer/MesherStub.hpp"
#include "visualizer/SteppableController.hpp"
#include "visualizer/Visualizer.hpp"

#include "common/meshes.hpp"
#include "configuration/ConfigFileParser.hpp"
#include "configuration/CommandLineParser.hpp"
#include "milling/MillingAlgorithm.hpp"
#include "milling/Stock.hpp"
#include "milling/cutters.hpp"
#include "visualizer/MillerRunnable.hpp"
#include "visualizer/MesherStub.hpp"
#include "meshing/StockMesher.hpp"

using namespace std;

Visualizer::Visualizer() {

	u_int max_depth = clp.getMaxOctreeHeight();
	std::string configFile = clp.getConfigFile();

	ConfigFileParser cfp(configFile);

	StockMesher::Ptr stockMesher = boost::make_shared< StockMesher >();
	Stock::Ptr stock = boost::make_shared< Stock >(*cfp.getStockDescription(), max_depth, stockMesher);
	Cutter::ConstPtr cutter = Cutter::buildCutter(*cfp.getCutterDescription());

	MillingAlgorithmConf millingConf(stock, cutter, cfp.CNCMoveBegin(), cfp.CNCMoveEnd());

	MillingSignaler::Ptr signaler = boost::make_shared< MillingSignaler >();
	SteppableController::Ptr controller = boost::make_shared< SteppableController >();

	MillingAlgorithm::Ptr algorithm = boost::make_shared< MillingAlgorithm >(millingConf);

	MillerRunnable miller(controller, signaler, algorithm);
	MesherStub mesher(signaler, stock);

	model = boost::shared_ptr<new StockMesh()>;
	Display display(model);

	//boost::thread drawScene(boost::ref(draw, model));
	display.draw();

	boost::thread millerThrd(boost::ref(miller));
	boost::thread mesherThrd(boost::ref(mesher));

	/*while(controller->canStep()) {
		controller->stepOnce();
		boost::thread updateScene(boost::ref(update));
		updateScene.join();
	}*/

	millerThrd.join();
	mesherThrd.join();

	//drawScene.join();
}

Visualizer::~Visualizer() {
	// TODO Auto-generated destructor stub
}

/* end draw() */


