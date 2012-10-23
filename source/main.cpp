#include <iostream>
#include <typeinfo>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <limits>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assign.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/thread.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "configuration/ConfigFileParser.hpp"
#include "configuration/CommandLineParser.hpp"
#include "milling/MillingAlgorithm.hpp"
#include "milling/Stock.hpp"
#include "milling/cutters.hpp"
#include "visualizer/MillerRunnable.hpp"
#include "visualizer/Display.hpp"
#include "meshing/StockMesher.hpp"

using namespace std;
using namespace Eigen;

int main(int argc, const char **argv) {
	
	/* 
	 * ******** ConfigFileParser TEST ********
	 */
//	ConfigFileParser confManager("testPositions.txt");
//	cout << "file read" << endl;
//	CutterDescriptionPtr cutter = confManager.getCutterDescription();
//	cout << "Cutter: " << cutter->getGeometry()->getType() << endl;
//	RectCuboidPtr stock = confManager.getStockDescription()->getGeometry();
//	cout << "Stock: " << stock->X << "," << stock->Y << "," << stock->Z << endl;
//	
//	CNCMoveIterator it = confManager.CNCMoveBegin();
//	cout << "first end? " << (it == confManager.CNCMoveEnd()) << endl;
//	cout << "tool: " << it->CUTTER << "|| product: " << it->STOCK << endl;
//	cout << "aft -> end? " << (it == confManager.CNCMoveEnd()) << endl;
//	
//	it++;
//	cout << "final end (should be true)? " << (it == confManager.CNCMoveEnd()) << endl;
//	
//	it = confManager.CNCMoveBegin();
//	cout << "tool: " << it->CUTTER << "|| product: " << it->STOCK << endl;
//	
//	long count = 0;
//	for(CNCMoveIterator it2 = confManager.CNCMoveBegin(); it2 != confManager.CNCMoveEnd(); it2++) {
//		count++;
//	}
//	cout << "There are " << count << " moves" << endl;
	
	/* 
	 * ******** CommandLineParser TEST ********
	 */
//	CommandLineParser clp(argc, argv);
//	cout << "usage printing begin ----" << endl;
//	clp.printUsage(cout);
//	cout << "---- end usage printing" << endl;
//	cout << "help? " << clp.isHelpAsked() << endl;
//	cout << "max height: " << clp.getMaxOctreeHeight() << endl;
//	cout << "config file: " << clp.getConfigFile() << endl;
	
	/* 
	 * ******** SimpleBox TEST ********
	 */
//	SimpleBox b1(Vector3d(2, 2, 2));
//	Translation3d trans(-1, 2, 0);
//	Matrix3d rot; rot = AngleAxisd(0.5 * M_PI, Vector3d::UnitZ())
//			* AngleAxisd(-0.5 * M_PI, Vector3d::UnitY())
//			* AngleAxisd(0, Vector3d::UnitX());
//	
//	CornerIterator cit;
//	
//	cout << "Simple box ostream: " << b1 << endl;
//	
//	cout << "All corners:" << endl;
//	for(cit = CornerIterator::begin(); cit != CornerIterator::end(); ++cit) {
//		cout << "#" << *cit << ": " << b1.getCorner(*cit).transpose() << endl;
//	}
//	
//	Isometry3d isomTrans(trans);
//	cout << "All corners translated of " << trans.translation().transpose() << endl;
//	for (cit = CornerIterator::begin(); cit != CornerIterator::end(); ++cit) {
//		cout << "#" << *cit << ": " << b1.getCorner(*cit, isomTrans).transpose() << endl;
//	}
//	cout << "... now given as matrix:" << endl;
//	cout << *b1.getCorners(isomTrans) << endl;
//	
//	Isometry3d isomRot(rot);
//	cout << "All corners rotated of " << endl << rot << endl;
//	for (cit = CornerIterator::begin(); cit != CornerIterator::end(); ++cit) {
//		cout << "#" << *cit << ": " << b1.getCorner(*cit, isomRot).transpose() << endl;
//	}
//	cout << "... now given as matrix:" << endl;
//	cout << *b1.getCorners(isomRot) << endl;
//	
//	Isometry3d isomRotTrans = isomRot * isomTrans;
//	cout << "All corners rot*trans of " << endl << isomRotTrans.matrix() << endl;
//	for (cit = CornerIterator::begin(); cit != CornerIterator::end(); ++cit) {
//		cout << "#" << *cit << ": " << b1.getCorner(*cit, isomRotTrans).transpose() << endl;
//	}
//	cout << "... now given as matrix:" << endl;
//	cout << *b1.getCorners(isomRotTrans) << endl;
//	
//	Isometry3d isomTransRot = isomTrans * isomRot;
//	cout << "All corners trans*rot of " << endl << isomTransRot.matrix() << endl;
//	for (cit = CornerIterator::begin(); cit != CornerIterator::end(); ++cit) {
//		cout << "#" << *cit << ": " << b1.getCorner(*cit, isomTransRot).transpose() << endl;
//	}
//	cout << "... now given as matrix:" << endl;
//	cout << *b1.getCorners(isomTransRot) << endl;
	
	
	/* 
	 * ******** ShiftedBox & Voxel TEST ********
	 */
////	SimpleBox::Ptr box = boost::make_shared< SimpleBox >(Vector3d(2, 2, 2));
////	Translation3d boxShift(1, 1, 1);
////	
////	ShiftedBox sbox(box, boxShift);
////	
////	cout << "shiftbox outstream: " << sbox << endl;
////	
////	Translation3d anotherShift(-1, -1, -1);
////	ShiftedBox anotherSbox = sbox.getShifted(anotherShift);
////	cout << "anotherSbox outstream: " << anotherSbox << endl;
////	
////	cout << "Sbox voxel: " << endl << sbox.getVoxel() << endl;
////	cout << "anotherSbox voxel: " << endl << anotherSbox.getVoxel() << endl;
//	
//	// SUBTEST: INTERSECTION
//	Vector3d extentsSBox(2, 4, 2), extentsIntBox(1, 2, 2);
//	
//	SimpleBox::Ptr tmpBox = boost::make_shared< SimpleBox >(extentsSBox);
//	
//	// some test shift box translations, uncomment required one
////	Translation3d sboxTranslation(Translation3d::Identity());
//	Translation3d sboxTranslation(1, 2, 0);
//	
//	ShiftedBox sboxInt(tmpBox, sboxTranslation);
//	
//	SimpleBox::Ptr intBox = boost::make_shared< SimpleBox >(extentsIntBox);
//	
//	// some test translations, uncomment required one
////	Translation3d intBoxOrgTras(Translation3d::Identity());
////	Translation3d intBoxOrgTras(0, 0, 3);
////	Translation3d intBoxOrgTras(1, 2, 0);
////	Translation3d intBoxOrgTras(1, -2, 0);
//	Translation3d intBoxOrgTras(2, 3, 0);
//	
//	Matrix3d intBoxRot;
//	// some test rotations, uncomment the required one
////	intBoxRot = AngleAxisd::Identity();
////	intBoxRot = AngleAxisd(0.5 * BOOST_PI, Vector3d::UnitZ());
//	intBoxRot = AngleAxisd(-0.25 * BOOST_PI, Vector3d::UnitZ());
//
//	Isometry3d tmpTrans(intBoxOrgTras), tmpRot(intBoxRot);
//	cout << "tmpTrans: " << endl << tmpTrans.matrix() << endl;
//	cout << "tmpRot: " << endl << tmpRot.matrix() << endl;
//	
//	Isometry3d rotoTrans; rotoTrans = tmpTrans * tmpRot;
//	
//	cout << "sboxInt: " << sboxInt << endl;
//	cout << "intersecting box: " << *intBox << endl;
//	cout << "intBox translation: " << intBoxOrgTras.translation().transpose() << endl;
//	cout << "rotoTrans trans: " << rotoTrans.translation().transpose() << endl; 
//	cout << "intBox rotation: " << endl << intBoxRot << endl;
//	cout << "rotoTrans rot: " << endl << rotoTrans.rotation() << endl;
//	cout << "rotoTrans matrix: " << endl << rotoTrans.matrix() << endl;
//	
//	cout << "point in intBox basis (0, 0, 0) => " << (rotoTrans * Vector3d(0, 0, 0)).transpose() << endl;
//	cout << "point in intBox basis (1, 2, 0) => " << (rotoTrans * Vector3d(1, 2, 0)).transpose() << endl;
//	
//	cout << "is intersecting? " << sboxInt.isIntersecting(intBox, rotoTrans, true) << endl;
	
	/*
	 * ROTOTRASLATION TEST
	 */
//	EulerAngles rotX(0.5 * M_PI, 0, 0);
//	EulerAngles rotY(0, 0.5 * M_PI, 0);
//	EulerAngles rotZ(0, 0, 0.5 * M_PI);
//	EulerAngles rot(0, -0.5 * M_PI, 0.5 * M_PI);
//	Point3D trasl(1, -2, 3);
//	
//	Rototraslation traslRotX(trasl, rotX);
//	cout << "traslRotX: " << endl << traslRotX.asEigen().matrix() << endl;
//	cout << "(1, 2, 3) in rotrasl basis become in world basis: " <<
//			(traslRotX.asEigen() * Vector3d(1, 2, 3)).transpose() << endl;
//	
//	Rototraslation traslRotY(trasl, rotY);
//	cout << "traslRotY: " << endl << traslRotY.asEigen().matrix() << endl;
//	cout << "(1, 2, 3) in rotrasl basis become in world basis: " <<
//			(traslRotY.asEigen() * Vector3d(1, 2, 3)).transpose() << endl;
//	
//	Rototraslation traslRotZ(trasl, rotZ);
//	cout << "traslRotZ: " << endl << traslRotZ.asEigen().matrix() << endl;
//	cout << "(1, 2, 3) in rotrasl basis become in world basis: " <<
//			(traslRotZ.asEigen() * Vector3d(1, 2, 3)).transpose() << endl;
//	
//	Rototraslation traslRot(trasl, rot);
//	cout << "traslRot: " << endl << traslRot.asEigen().matrix() << endl;
//	cout << "(1, 2, 3) in rotrasl basis become in world basis: " <<
//			(traslRot.asEigen() * Vector3d(1, 2, 3)).transpose() << endl;
	
	
	/* 
	 * ******** Octree getIntersectingLeaves TEST ********
	 */
//	
//	cout << "inizio" << endl;
//	
//	u_int MAX_DEPTH = 2;
//	Vector3d size(2.0, 2.0, 2.0);
//	
//	Octree<MyData> octree(size);
//	
//	cout << "octree creato" << endl;
//	
//	SimpleBox b1(Vector3d(1.0, 1.0, 2.0));
//	Vector3d traslation(1, 1, 1.5);
// //	Vector3d traslation(Vector3d::Zero());
//	Matrix3d rotation;
//	rotation = AngleAxisd(0, Vector3d::UnitX())
//			* AngleAxisd(0, Vector3d::UnitY())
//			* AngleAxisd(0, Vector3d::UnitZ());
//	
//	cout << "varie box/trasl/rot creati" << endl;
//	
//	Octree<MyData>::LeavesDequePtr leaves = octree.getIntersectingLeaves(b1, traslation, rotation);
//	
//	cout << "intersecting leaves fatto" << endl;
//	
//	cout << "found leaves: " << leaves->size() << endl;
//	
//	Octree<MyData>::LeavesDeque::iterator leavesIt = leaves->begin();
//	int number = 1;
//	for (; leavesIt != leaves->end(); ++leavesIt) {
//		MyData d; d.value = number++;
//		(**leavesIt).setData(d);
//	}
//	
//	cout << "setting data fatta" << endl;
//	
//	Octree<MyData>::DataDequePtr data = octree.getStoredData();
//	
//	cout << "get stored data fatta" << endl;
//	
//	cout << "Data: ";
//	Octree<MyData>::DataDeque::iterator dataIt = data->begin();
//	for (; dataIt != data->end(); ++dataIt) {
//		cout << (*dataIt)->value << ", ";
//	}
//	cout << endl;
//	
//	std::vector< Octree<MyData>::LeafPtr > addedLeaves;
//	for (leavesIt = leaves->begin(); leavesIt != leaves->end(); ++leavesIt) {
//		if ((*leavesIt)->getDepth() >= MAX_DEPTH) {
//			continue;
//		}
//		
//		Octree<MyData>::LeavesDequePtr newLeaves = octree.pushLevel(*leavesIt);
//		addedLeaves.insert(addedLeaves.end(), newLeaves->begin(), newLeaves->end());
//	}
//	octree.notifyChanges();
//	
//	
//	leaves = octree.getIntersectingLeaves(b1, traslation, rotation);
//		
//	cout << "intersecting leaves 2 fatto" << endl;
//	
//	cout << "found leaves 2: " << leaves->size() << endl;
//	
//	for (leavesIt = leaves->begin(); leavesIt != leaves->end(); ++leavesIt) {
//		MyData d; d.value = number++;
//		(**leavesIt).setData(d);
//	}
//	
//	cout << "setting data 2 fatta" << endl;
//	
//	data = octree.getStoredData();
//	
//	cout << "get stored data 2 fatta" << endl;
//	
//	dataIt = data->begin();
//	cout << "Data 2: ";
//	for (; dataIt != data->end(); ++dataIt) {
//		cout << (*dataIt)->value << ", ";
//	}
//	cout << endl;

	
	/* 
	 * ******** Stock intersect TEST ********
	 */
//	CommandLineParser clp(argc, argv);
//	
//	if (clp.isHelpAsked()) {
//		clp.printUsage(cout);
//		return 0;
//	}
//	
//	u_int MAX_DEPTH = clp.getMaxOctreeHeight();
//	
//	cout << "inizio con MAX_DEPTH=" << MAX_DEPTH << endl;
//	
//	RectCuboidPtr rectCuboid = boost::make_shared< RectCuboid >(2.0, 2.0, 2.0);
//	StockDescription stockDesc(rectCuboid);
//	
//	Stock stock(stockDesc, MAX_DEPTH);
//	
//	cout << "stock creato" << endl;
//	
//	GeometryPtr cylinderPtr = boost::make_shared< Cylinder >(1.0 , 2.0);
//	Color cutterColor;
//	CutterDescription cutterDesc(cylinderPtr, cutterColor);
//	
//	Cutter::CutterPtr cutter = Cutter::buildCutter(cutterDesc);
//	
//	cout << "cutter creato" << endl;
//	
//	Vector3d traslation(1, 1, 1.5);
// //	Vector3d traslation(Vector3d::Zero());
//	Matrix3d rotation;
//	rotation = AngleAxisd(0, Vector3d::UnitX())
//			* AngleAxisd(0, Vector3d::UnitY())
//			* AngleAxisd(0, Vector3d::UnitZ());
//	
//	cout << "trasl/rot cutter rispetto a stock creati" << endl;
//	
//	cout << "now performing intersection" << endl;
//	IntersectionResult res = stock.intersect(cutter, traslation, rotation);
//	
//	cout << "Results: " << res << endl;
//	
//	cout << "Resulting stock: " << stock << endl;
	
	
	/* 
	 * ******** Milling algorithm TEST ********
	 */
//	CommandLineParser clp(argc, argv);
//	
//	if (clp.isHelpAsked()) {
//		clp.printUsage(cout);
//		return 0;
//	}
//	
//	u_int max_depth = clp.getMaxOctreeHeight();
//	std::string configFile = clp.getConfigFile();
//	
//	ConfigFileParser cfp(configFile);
//	Stock::Ptr stock = boost::make_shared< Stock >(*cfp.getStockDescription(), max_depth);
//	Cutter::ConstPtr cutter = Cutter::buildCutter(*cfp.getCutterDescription());
//	
//	MillingAlgorithm millingAlg(stock, cutter, cfp.CNCMoveBegin(), cfp.CNCMoveEnd());
//	
//	cout << millingAlg << std::endl;
//	cout << "Start milling:" << endl << MillingResult::getPrintHeader() << endl;
//	while(!millingAlg.hasFinished()) {
//		MillingResult res = millingAlg.step();
//		cout << res << std::endl;
//		if (clp.verbosityLevel() > 0)
//			cout << millingAlg << std::endl;
//	}
	
	

	/* 
	 * ******** Threading TEST ********
	 */
	CommandLineParser clp(argc, argv);
	
	if (clp.isHelpAsked()) {
		clp.printUsage(cout);
		return 0;
	}
	
	u_int max_depth = clp.getMaxOctreeHeight();
	std::string configFile = clp.getConfigFile();
	
	ConfigFileParser cfp(configFile);
	
	StockMesher::Ptr stockMesher = boost::make_shared< StockMesher >(*cfp.getStockDescription());
	Stock::Ptr stock = boost::make_shared< Stock >(*cfp.getStockDescription(), max_depth, stockMesher);
	Cutter::Ptr cutter = Cutter::buildCutter(*cfp.getCutterDescription());
	
	MillingAlgorithmConf millingConf(stock, cutter, cfp.CNCMoveBegin(), cfp.CNCMoveEnd());
	
	MillingSignaler::Ptr signaler = boost::make_shared< MillingSignaler >();
	SteppableController::Ptr controller = boost::make_shared< SteppableController >(true);
	
	MillingAlgorithm::Ptr algorithm = boost::make_shared< MillingAlgorithm >(millingConf);
	
	MillerRunnable miller(controller, signaler, algorithm);
	
	cout << "Setup info: " << endl
			<< "\tPosition file: " << configFile << endl
			<< "\tCutter: " << *cutter << endl
			<< "\tStock: " << *stock << endl
			;
	
	boost::thread millerThrd(boost::ref(miller));
	
	Display display(stock, cutter, signaler, controller);
	display.draw();
	
	cout << "exiting drawer & stopping miller" << endl;
	controller->stop();
	
	cout << "exiting main & waiting for miller thread..." << endl;
	millerThrd.join();
	cout << "miller ended!" << endl;
//	mesherThrd.join();
//	cout << "meshaer ended! Bye..." << endl;
	
	
	/* ****************
	 * *** MAIN *******
	 */
//	CommandLineParser clp(argc, argv);
//	
//	if (clp.isHelpAsked()) {
//		clp.printUsage(cout);
//		return 0;
//	}
	
	
	return 0;
}




