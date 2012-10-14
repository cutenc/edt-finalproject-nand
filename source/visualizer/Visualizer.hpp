/*
 * Visualizer.hpp
 *
 *  Created on: 18/set/2012
 *      Author: alberto
 */

#ifndef VISUALIZER_HPP_
#define VISUALIZER_HPP_

#include <boost/thread.hpp>
#include <iostream>
#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osgViewer/Viewer>
#include <vector>

#include "common/Mesh.hpp"
#include "meshing/Meshing.hpp"
#include "milling/Cutter.hpp"
#include "milling/MillingResult.hpp"
#include "milling/Stock.hpp"
#include "MillingSignaler.hpp"
#include "Display.hpp"

class Visualizer : public CyclicRunnable {

	boost::shared_ptr<StockMesh> model;
	/*Stock::Ptr stock;
	Cutter::Ptr cutter;*/
	bool finished;
	int noOfPoints, noOfTriangles;

	public:
		Visualizer();
		virtual ~Visualizer();

};

#endif /* VISUALIZER_HPP_ */
