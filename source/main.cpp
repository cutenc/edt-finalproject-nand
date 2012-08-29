#include <iostream>
#include <typeinfo>
#include <map>
#include <string>
#include <fstream>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assign.hpp>

#include <Eigen/Geometry>

#include "common/Point3D.hpp"
#include "common/Rototraslation.hpp"
#include "configuration/Geometry.hpp"
#include "configuration/StockDescription.hpp"
#include "configuration/ConfigFileParser.hpp"
#include "configuration/CommandLineParser.hpp"
#include "milling/SimpleBox.hpp"
#include "milling/Octree.hpp"

using namespace std;
using namespace Eigen;

void printA(ostream &c) { c << "a" << endl; }
void printB(ostream &c) { c << "b" << endl; }

int main(int argc, const char **argv) {
	
	/* 
	 * ******** ConfigFileParser TEST ********
	 */
//	ConfigFileParser confManager("positions.txt");
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
//	SimpleBox b1(2, 4, 1), b2(2, 4, 1);
//	
////	Vector3d traslation(3.00000000001, 0, 0);
//	Vector3d traslation(1, 3, 1+2*numeric_limits<double>::epsilon());
//	
//	Matrix3d rot;
//	rot = AngleAxisd(0, Vector3d::UnitX())
//			* AngleAxisd(0, Vector3d::UnitY())
//			* AngleAxisd(0.25 * M_PI, Vector3d::UnitZ());
//	
//	cout << "rotation: " << endl << rot << endl;
//	cout << "traslation: " << traslation.transpose() << endl;
//	
//	bool collision = b1.isIntersecting(b2, traslation, rot);
//	
//	cout << "b1 and b2 are " << ((collision) ? "" : "NOT ") << "colliding" << endl;
//	
//	cout << "corners of " << b1 << ":" << endl;
//	for (CornerIterator it = CornerIterator::begin(); it != CornerIterator::end(); ++it) {
//		cout << "corn#" << *it << " b1: " << b1.getCorner(*it).transpose()
//				<< "; b2: " << b2.getCorner(*it, traslation, rot).transpose() << endl;
//	}
	
	/* 
	 * ******** Octree getIntersectingLeaves TEST ********
	 */
	Octree<int> octree(2, Vector3d(2, 2, 2));
	
	SimpleBox b1(1, 1, 2);
	Vector3d traslation(.5, .5, .5);
	Matrix3d rotation;
	rotation = AngleAxisd(0, Vector3d::UnitX())
			* AngleAxisd(0, Vector3d::UnitY())
			* AngleAxisd(0, Vector3d::UnitZ());
	
	Octree<int>::LeavesVectorPtr leaves = octree.getIntersectingLeaves(b1, traslation, rotation);
	
	cout << "found leaves: " << leaves->size() << endl;
	cout << "leaf index: " << ((int)leaves->front()->getChildIdx()) << endl;
	
	
	
	/* ****************
	 * *** MAIN *******
	 */
//	CommandLineParser clp(argc, argv);
//	
//	if (clp.isHelpAsked()) {
//		clp.printUsage(cout);
//		return 0;
//	}
//	
	
	
	return 0;
}




