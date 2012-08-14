#include <iostream>
#include <typeinfo>

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

#include "common/Point3D.hpp"
#include "common/Rototraslation.hpp"
#include "configuration/Geometry.hpp"
#include "configuration/StockDescriptor.hpp"

using namespace std;

int main(int argc, char **argv) {
	
	Point3D p(1, 2, 3);
	cout << "p(" << p.X << "," << p.Y << "," << p.Z << ")" << endl;
	
	StockDescription stock(GeometryPtr(new Sphere(1.0)));
	
	// cout << "Sphere type: " << typeid(s).name() << endl;
	
	cout << "cast attempt" << endl;
	const Sphere *sr2 = stock.getGeometry()->getAs<Sphere>();
	
	cout << "sr2: " << sr2->getType() << " radius: " << sr2->RADIUS << endl;
	
	Rototraslation rt;
	cout << "default rototraslation: " << rt << endl;
	
	return 0;
}




