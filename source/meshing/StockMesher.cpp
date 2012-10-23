/*
 * StockMesher.cpp
 *
 *  Created on: 22/set/2012
 *      Author: socket
 */

#include "StockMesher.hpp"

#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>

#include <osg/Geode>
#include <osg/ShapeDrawable>

#include "visualizer/VisualizationUtils.hpp"
#include "milling/ShiftedBox.hpp"
#include "MeshingUtils.hpp"

// TODO REMOVE THESE INCLUDES
#include <iostream>

StockMesher::StockMesher(const StockDescription& stock) :
		HALF_EXTENTS(stock.getGeometry()->X * 0.5,
				stock.getGeometry()->Y * 0.5,
				stock.getGeometry()->Z * 0.5
				),
		MESH(new osg::Group),
		cubeCache()
{
}

StockMesher::~StockMesher() {
}

Mesh::Ptr StockMesher::buildMesh(const StoredData &data) {
	
	// TODO REMOVE cout
	std::cout << "dataSize: " << data.getData()->size()
			<< "; deleteSize: " << data.getDeleted()->size() << std::endl;
	
	/* first analyze new & updated data then deleted one: this way even
	 * if a leaf appears in both queue it will be processed correctly
	 */
	
	// data queue analysis
	StoredData::VoxelData::iterator dataIt = data.getData()->begin();
	for(; dataIt != data.getData()->end(); ++dataIt) {
		
		/* here we CANNOT assert(!dataIt->second->isContained()) because
		 * even if this leaf has been placed in these queue because it
		 * was intersecting & not contained, it may have been processed
		 * again by the miller thread that deleted all its corners
		 */
		
		if(dataIt->second->hasGraphics()) {
			/* current leaf is already displayed, nothing to do here.
			 * This behavior is different from a MarchingCube mesher because
			 * it has to recalculate the entire mesh for the voxel & update
			 * geode pointer
			 */
			continue;
		}
		
		ShiftedBox::ConstPtr sbox = dataIt->first;
		
		if(dataIt->second->isIntersecting() ||
				MeshingUtils::isBorderVoxel(HALF_EXTENTS, *sbox)) {
			// we have to build the cube & append it to the master geode
			
			// get cube from cache and create a surrounding Geode
			osg::ref_ptr< osg::ShapeDrawable > cube = cubeCache.getCube(sbox->getSimpleBox());
			osg::Geode *geode = new osg::Geode;
			geode->addDrawable(cube.get());
			
			// translate it
			osg::PositionAttitudeTransform *pat = new osg::PositionAttitudeTransform;
			pat->setPosition(MeshingUtils::toOsg(sbox->getShift().translation()));
			pat->addChild(geode);
			
			// set built voxel mesh both to MESH and VoxelInfo
			MESH->addChild(pat);
			dataIt->second->setGraphics(pat);
		}
	}
	
	// deleted queue analysis
	StoredData::DeletedData::const_iterator deletedIt = data.getDeleted()->begin();
	for (; deletedIt != data.getDeleted()->end(); ++deletedIt) {
		// TODO VERY VERY INEFFICIENT, we have to store even child position (or maybe only)
		MESH->removeChild((*deletedIt)->getGraphics().get());
	}
	
	return boost::make_shared< Mesh >(MESH.get());
}
