/**
 * @file MarchingCubeMesherCallback.hpp
 *
 * Created on: 29/ott/2012
 * Author: socket
 *
 * DO NOT INCLUDE THIS FILE DIRECTLY, YOU SHOULD USE
 * leaf_node_callbacks.hpp"
 *
 * Marching Cubes algorithm is implemented here.
 * Tables are borrowed from http://paulbourke.net/geometry/polygonise/ (many thanks)
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
	/** contains the possible combinations of triangle faces */
	static const int edgeTable[256];
	/** for every possible combination of triangle faces, indicates the vertices and edges involved in its construction */
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
	/**
	 * constructor
	 *
	 * @param desc : the stock characteristics
	 */
	MarchingCubeMesherCallback(const StockDescription &desc);
	
	/**
	 * MC algo.
	 * Creates the OSG object to be inserted into the scene tree, representing the cut voxel
	 *
	 * @param data : the infos of the voxel to be translated into graphic
	 * @return the reference of the graphic object to be inserted into the mesh
	 */
	virtual osg::ref_ptr< osg::Node > buildNode(const LeafNodeData &data);
	
protected:
	/**
	 * destructor - empty
	 */
	virtual ~MarchingCubeMesherCallback();
	
private:
	
	/**
	 * calculates the midpoint of an edge
	 *
	 * @param grid : the voxel to be cut, where the edge belong
	 * @param edgeIdx : the edge of the voxel where the midpoint lies
	 * @return the coords of the midpoint
	 */
	Eigen::Vector3d vertInterp(const MeshingVoxel &grid, int edgeIdx);
	
};


#endif /* MARCHINGCUBEMESHERCALLBACK_HPP_ */
