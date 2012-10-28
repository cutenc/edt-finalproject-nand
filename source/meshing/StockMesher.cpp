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
	
	/* using an octree it's better to start freeing space and then appending
	 * new leaves. However if a leaf appears in both sequences there will be
	 * an incosistency: check this with an assert on the reference count
	 * of the graphic object (it must be always 2: one ref for the scene
	 * and another for the voxelInfo)
	 */
	
	StoredData::VoxelData::iterator dataIt = data.getData()->begin();
	
	// deleted queue analysis
	StoredData::DeletedData::const_iterator deletedIt = data.getDeleted()->begin();
	for (; deletedIt != data.getDeleted()->end(); ++deletedIt) {
		VoxelInfo::Ptr vinfo = *deletedIt;
		if (!vinfo->hasGraphics()) {
			continue;
		}
		
		u_int idx = vinfo->getGraphics().second;
		
		assert(MESH->getChild(idx) == vinfo->getGraphics().first.get());
	}
		

	
	// data queue analysis
	for(; dataIt != data.getData()->end(); ++dataIt) {
		
		/* here we CANNOT assert(!dataIt->second->isContained()) because
		 * even if this leaf has been placed in these queue because it
		 * was intersecting & not contained, it may have been processed
		 * again by the miller thread that deleted all its corners
		 */
		
		if(dataIt->vInfo->hasGraphics()) {
			/* current leaf is already displayed, nothing to do here.
			 * This behavior is different from a MarchingCube mesher because
			 * it has to recalculate the entire mesh for the voxel & update
			 * geode pointer
			 */
			
			// one ref for the scene and one ref for the vInfo
			assert(dataIt->vInfo->getGraphics().first->referenceCount() == 2);
			continue;
		}
		
		const ShiftedBox::ConstPtr &sbox = dataIt->sBox;
		
		if(dataIt->vInfo->isIntersecting() ||
				MeshingUtils::isBorderVoxel(HALF_EXTENTS, *sbox)) {
			// we have to build the cube & append it to the master geode
			
			// get cube from cache and create a surrounding Geode

			/*
			 * aggiungo io (Franz): chiamata a MC - spero sia nel punto giusto
			 */
			MeshingVoxel mv;
			int i;
			mv = MeshingVoxel(
						sbox.get()->simpleBox,
						this->getDistance(
								Eigen::Vector3d(0,0,0), // da dove?
								sbox.get()->simpleBox
						)
					);

			osg::ref_ptr<osg::Geode> geode = osg::ref_ptr<osg::Geode>(new osg::Geode);
			geode->addDrawable(Meshing::buildMesh(mv, MC_THRESHOLD));
			MESH->addChild(geode);

			//osg::ref_ptr< osg::ShapeDrawable > cube = cubeCache.getCube(sbox->getSimpleBox());
			//osg::Geode *geode = new osg::Geode;
			//geode->addDrawable(cube.get());
			
			// translate it
			osg::PositionAttitudeTransform *pat = new osg::PositionAttitudeTransform;
			pat->setPosition(MeshingUtils::toOsg(sbox->getShift().translation()));
			pat->addChild(geode);
			
			// set built voxel mesh both to MESH and VoxelInfo
			MESH->addChild(pat);
			dataIt->vInfo->setGraphics(pat, MESH->getNumChildren() - 1);
			assert(MESH->getChild(MESH->getNumChildren() - 1) == pat);
		}
	}
	
	return boost::make_shared< Mesh >(MESH.get());
}

double* StockMesher::getDistance(const Eigen::Vector3d point, SimpleBox::Ptr sbp) {

	double weights[8] = {
			VoxelInfo::DEFAULT_INSIDENESS(),VoxelInfo::DEFAULT_INSIDENESS(),
			VoxelInfo::DEFAULT_INSIDENESS(),VoxelInfo::DEFAULT_INSIDENESS(),
			VoxelInfo::DEFAULT_INSIDENESS(),VoxelInfo::DEFAULT_INSIDENESS(),
			VoxelInfo::DEFAULT_INSIDENESS(),VoxelInfo::DEFAULT_INSIDENESS()
	};

	int i;
	for (i = 0; i < 8; i++) {
		if (point[2] < 0)
				return boost::math::changesign(CommonUtils::INFINITE());

		double halfDim = HALF_EXTENTS;//sbp.get()->getHalfExtent();


		/* following implicit function given in "Adaptive NC Simulation
		 * for Multi-axis Solid Machining" for a flat endmill aligned with
		 * Z-axis we can write:
		 */
		double firstTerm = fabs((double)point[2] - halfDim) - halfDim;

		/* in order to reduce floating point approximation errors we will
		 * rewrite
		 * double secondTerm = sqr(point.getX()) + sqr(point.getY()) - SQR_RADIUS;
		 * in a more floating-point-friendly way.
		 *
		 * http://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
		 */
		double secondTerm = boost::math::pow< 2 >((double)point[0])
				+ (point[1] + RADIUS) * (point[1] - RADIUS);

		double distance = fmax(firstTerm, secondTerm);

		/* I have to add a minus sign because in paper specification
		 * distance <0 means "inside surface" and >0 "outside surface" but,
		 * in our specification <0 means "outside" and >0 "inside"
		 */
		//return
		weights[i] = boost::math::changesign(distance);
	}

	return weights;
}
