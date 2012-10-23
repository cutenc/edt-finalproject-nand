/*
 * Meshing.hpp
 *
 *  Created on: 17/ott/2012
 *      Author: alberto
 */

#ifndef MESHING_HPP_
#define MESHING_HPP_

#include <math.h>

#include <osg/Array>
#include <osg/Node>
#include <osg/Geometry>

#include "Triangle.hpp"
#include "Voxel.hpp"
#include "VoxelInfo.hpp"

//using namespace std;

class Cell {

public:
	Cell();
	~Cell();

	osg::Vec3f *p;
	double *val;

};

class Meshing {
public:
	Meshing();
	virtual ~Meshing();

	static const int edgeTable[256];

	static const int triTable[256][16];

	inline static double cutterThresholdError() {
		return 0.00001;
	};

	static osg::Vec3f VertInterp(const double cutterThreshold,
					      const osg::Vec3f p1,
						  const osg::Vec3f p2,
						  const double valp1,
						  const double valp2);
	osg::ref_ptr<osg::Geometry> buildMesh(MeshingVoxel grid, double cutterThreshold);

};

#endif /* MESHING_HPP_ */
