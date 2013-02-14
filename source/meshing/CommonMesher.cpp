/**
 * CommonMesher.cpp
 *
 *  Created on: 26/ott/2012
 *      Author: socket
 */

#include "CommonMesher.hpp"

#include <osg/BoundingBox>

#include "MeshingUtils.hpp"

CommonMesher::CommonMesher(const StockDescription& stock,
		LeafNodeCallback *lnc, unsigned int maxLeafSize, unsigned int maxDepth) :
		HALF_EXTENTS(stock.getGeometry()->asEigen() * 0.5),
		meshOctree(
				osg::BoundingBoxd(
						-stock.getGeometry()->asOsg() * 0.5,
						stock.getGeometry()->asOsg() * 0.5
				),
				lnc,
				maxLeafSize,
				maxDepth
		)
{ }

CommonMesher::~CommonMesher() {
}

Mesh::Ptr CommonMesher::buildMesh(const StoredData &data) {
	
	/* using an octree it's better to start freeing space and then appending
	 * new leaves. However if a leaf appears in both sequences there will be
	 * an inconsistency: check this with an assert on the reference count
	 * of the graphic object (it must be always 2: one ref for the scene
	 * and another for the voxelInfo)
	 */
	
	// deleted queue analysis
	StoredData::DeletedData::const_iterator deletedIt = data.getDeleted()->begin();
	for (; deletedIt != data.getDeleted()->end(); ++deletedIt) {
		VoxelInfo::Ptr vinfo = *deletedIt;
		
		if (!vinfo->hasGraphics()) {
			continue;
		}
		
		meshOctree.removeData(vinfo->getGraphics());
	}
	
	
	// data queue analysis
	StoredData::VoxelData::iterator dataIt = data.getData()->begin();
	for(; dataIt != data.getData()->end(); ++dataIt) {
		
		/* here we CANNOT assert(!dataIt->vinfo->isContained()) because
		 * even if this leaf has been placed in these queue because it
		 * was intersecting & not contained, it may have been processed
		 * again (during the current meshing) by the miller thread, that
		 * deleted all its corners
		 */
		
		if(dataIt->vinfo->hasGraphics()) {
			/* current data is already displayed, so we have to update it!
			 */
			meshOctree.updateData(dataIt->vinfo->getGraphics(), *dataIt);
			
		} else {
			// current data has not been displayed yet...
			const ShiftedBox::ConstPtr &sbox = dataIt->sbox;
			
			if(dataIt->vinfo->isIntersecting() ||
					MeshingUtils::isBorderVoxel(HALF_EXTENTS, *sbox)) {
				// ... and we have to display it, so add to the data structure
				
				bool inserted = meshOctree.addData(*dataIt);
				assert(inserted);
			}
		}
	}
	
	return boost::make_shared< Mesh >(meshOctree.getRoot());
}
