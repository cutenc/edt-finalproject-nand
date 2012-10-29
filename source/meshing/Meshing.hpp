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

#include "meshing/MeshingVoxel.hpp"
#include "milling/VoxelInfo.hpp"

//using namespace std;

class Meshing {
public:
	Meshing();
	virtual ~Meshing();

	static const int edgeTable[256];

	static const int triTable[256][16];

	static osg::ref_ptr<osg::Geometry> buildMesh(const MeshingVoxel &grid, double cutterThreshold);
	static osg::Vec3f VertInterp(const double cutterThreshold,
					      const osg::Vec3f &p1,
						  const osg::Vec3f &p2,
						  const double valp1,
						  const double valp2);

};

#endif /* MESHING_HPP_ */
