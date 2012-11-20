/*
 * MarchingCubeMesherCallback.hpp
 *
 *  Created on: 29/ott/2012
 *      Author: socket
 *      
 * DO NOT INCLUDE THIS FILE DIRECTLY, YOU SHOULD USE
 * "leaf_node_callbacks.hpp"
 */

#ifndef MARCHINGCUBEMESHERCALLBACK_HPP_
#define MARCHINGCUBEMESHERCALLBACK_HPP_

#include "meshing/LeafNodeCallback.hpp"

#include <cassert>

#include <Eigen/Geometry>

#include <osg/Array>

#include "configuration/StockDescription.hpp"
#include "MeshingVoxel.hpp"

class MarchingCubeMesherCallback : public LeafNodeCallback {
	
private:
	static const int edgeTable[256];
	static const int triTable[256][16];
	
	/**
	 * Tells which two corners are adjacent a given edge 
	 */
	static const int cornerAdjTable[12][2];
	
	const osg::ref_ptr< osg::Vec4Array > mcColorArray;
	
	const osg::ref_ptr< osg::Vec4Array > boxColorArray;
	const osg::ref_ptr< osg::Vec3Array > boxNormals;
	
	const Eigen::Vector3d STOCK_HALF_EXTENTS;
	
	Eigen::Vector3d tmpVertices[12];
	
public:
	MarchingCubeMesherCallback(const StockDescription &desc);
	
	virtual osg::ref_ptr< osg::Node > buildNode(const LeafNodeData &data);
	
protected:
	virtual ~MarchingCubeMesherCallback();
	
private:
	
	Eigen::Vector3d vertInterp(const MeshingVoxel &grid, int edgeIdx);
	
};


#endif /* MARCHINGCUBEMESHERCALLBACK_HPP_ */
