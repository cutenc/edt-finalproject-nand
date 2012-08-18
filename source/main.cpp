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

#include "common/Point3D.hpp"
#include "common/Rototraslation.hpp"
#include "configuration/Geometry.hpp"
#include "configuration/StockDescription.hpp"

#include "configuration/ConfigurationManager.hpp"

using namespace std;

void printA(ostream &c) { c << "a" << endl; }
void printB(ostream &c) { c << "b" << endl; }

int main(int argc, char **argv) {
	
//	ifstream ifs;
//	ifs.exceptions(ifstream::failbit | ifstream::badbit);
//	
//	ifs.open("positions.txt", ios_base::in);
//	
//	if (!ifs.is_open())
//		throw std::ios_base::failure("can't open given file");
//	
//	cout << "initial g: " << ifs.tellg() << endl;
//	
//	string line;
//	getline(ifs, line);
//	
//	cout << "after first readline g: " << ifs.tellg() << endl;
//	
//	cout << "Read line g[" <<ifs.tellg() << "] -> '"<< line << "'" << endl;
//	cout << "line size: " << line.size() << endl;
//	
//	streamoff off = line.size();
//	cout << "offset: " << off;
//	ifs.seekg(-off, ios_base::cur);
//	
//	getline(ifs, line);
//	cout << "post rl1 g[" << ifs.tellg() << "] '" << line << "'" << endl;
//	getline(ifs, line);
//	cout << "post rl2 g[" << ifs.tellg() << "] '" << line << "'" << endl;
//	getline(ifs, line);
//	cout << "post rl3 g[" << ifs.tellg() << "] '" << line << "'" << endl;
	
	ConfigurationManager confManager("positions.txt");
//	cout << "file read" << endl;
//	CutterDescriptionPtr cutter = confManager.getCutterDescription();
//	cout << "Cutter: " << cutter->getGeometry()->getType() << endl;
//	RectCuboidPtr stock = confManager.getStockDescription()->getGeometry();
//	cout << "Stock: " << stock->X << "," << stock->Y << "," << stock->Z << endl;
	
	
	CNCMoveIterator it = confManager.CNCMoveBegin();
	cout << "first end? " << (it == confManager.CNCMoveEnd()) << endl;
	cout << "tool: " << it->CUTTER << "|| product: " << it->STOCK << endl;
	cout << "aft -> end? " << (it == confManager.CNCMoveEnd()) << endl;
	
	it++;
	cout << "final end (should be true)? " << (it == confManager.CNCMoveEnd()) << endl;
	
	it = confManager.CNCMoveBegin();
	cout << "tool: " << it->CUTTER << "|| product: " << it->STOCK << endl;
	
	long count = 0;
	for(CNCMoveIterator it2 = confManager.CNCMoveBegin(); it2 != confManager.CNCMoveEnd(); it2++) {
		count++;
	}
	cout << "There are " << count << " moves" << endl;
	
	return 0;
}




