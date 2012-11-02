/*
 * MeshOctree.cpp
 *
 *  Created on: 24/ott/2012
 *      Author: socket
 */

#include "MeshOctree.hpp"

#include <cassert>
#include <climits>

#include <osg/Group>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>

#include "visualizer/VisualizationUtils.hpp"
#include "MeshingUtils.hpp"
#include "BranchNodeData.hpp"
#include "milling/VoxelInfo.hpp"

MeshOctree::MeshOctree(const osg::BoundingBoxd& bbox, 
		LeafNodeCallback* nodeCallback, unsigned int dataPerLeaf, unsigned int maxDepth) :
	groupCallback(nodeCallback),
	maxLeafSize(dataPerLeaf), maxDepth(maxDepth)
{
	ROOT = createLeafGrp(bbox, 0).get();
	
	PROCESSERS[OctreeNodeData::BranchLODData] = &MeshOctree::processBranchGrp;
	PROCESSERS[OctreeNodeData::LeafLODData] = &MeshOctree::processLeafGrp;
}

MeshOctree::~MeshOctree() {
}

bool MeshOctree::addData(const GraphicData& data) {
	// build given element bounding box
	osg::BoundingBoxd bbox = data.sbox->asBoundingBox();
	
	OctreeNodeData *nodeData = MeshingUtils::getUserData< OctreeNodeData >(getRoot());
	if (!nodeData->isMyCompetence(bbox)) {
		return false;
	}
	
	unsigned int pidx = static_cast< unsigned int >(nodeData->getType());
	assert(pidx < 2);
	(this->*(PROCESSERS[pidx]))(getRoot(), bbox, data);
	
	return true;
}

void MeshOctree::updateData(const GraphicPointer& ref, const GraphicData& gdata) {
	LeafNodeData *lnd = MeshingUtils::getUserData< LeafNodeData >(ref.node.get());
	lnd->updateElm(ref.item, gdata);
	ref.node->setDataVariance(osg::Node::DYNAMIC);
}

void MeshOctree::removeData(const GraphicPointer& ref) {
	LeafNodeData *lnd = MeshingUtils::getUserData< LeafNodeData >(ref.node.get());
	lnd->deleteElm(ref.item);
	ref.node->setDataVariance(osg::Node::DYNAMIC);
}

osg::Group* MeshOctree::getRoot() {
	return this->ROOT.get();
}

osg::ref_ptr<osg::Group> MeshOctree::createLeafGrp(const osg::BoundingBoxd& bbox, unsigned char depth) {
	osg::ref_ptr< osg::Group > grp = new osg::Group;

	// create Group data & set it
	LeafNodeData *lData = new LeafNodeData(bbox, depth);
	grp->setUserData(lData);
	
	// set callback to the group
	grp->setDataVariance(osg::Node::DYNAMIC);
	grp->setUpdateCallback(groupCallback.get());
	
	// create child geode (stub) and add to it (because groupCallback expects it)
	grp->addChild(new osg::Geode);
	
	return grp.get();
}

osg::ref_ptr< LeafNodeData > MeshOctree::pushGrp(osg::Group* grp) {
	// replace old user data with new (branch) one
	osg::ref_ptr< LeafNodeData > oldData = MeshingUtils::getUserData< LeafNodeData >(grp);
	grp->setUserData(new BranchNodeData(oldData->getCompetenceBox(), oldData->getDepth()));
	
	// set data variance to STATIC & remove callback
	grp->setDataVariance(osg::Node::STATIC);
	grp->setUpdateCallback(NULL);
	
	// remove current grp only child
	assert(grp->getNumChildren() == 1);
	grp->removeChild(LeafNodeCallback::NODE_IDX, 1);
	
	// now create new 8 leaf group children adding them to the current group
	
	/* we have to create new bounding boxes:
	 * thanks to "OpenSceneGraph3 Cookbook" for the if-free code
	 */
	int s[3];
	osg::Vec3d extentSet[3] = {
		oldData->getCompetenceBox()._min,
		oldData->getCompetenceBox().center(),
		oldData->getCompetenceBox()._max
	};
	
	for ( s[0]=0; s[0]<2; ++s[0] ) {
		for ( s[1]=0; s[1]<2; ++s[1] ) {
			for ( s[2]=0; s[2]<2; ++s[2] ) {
				osg::Vec3 min, max;
				for ( int a=0; a<3; ++a ) {
					min[a] = (extentSet[s[a] + 0])[a];
					max[a] = (extentSet[s[a] + 1])[a];
				}
				
				osg::BoundingBoxd newBbox(min, max);
				osg::ref_ptr< osg::Group > newGrp = createLeafGrp(newBbox, oldData->getDepth() + 1);
				
				grp->addChild(newGrp.get());
			}
		}
	}
	
	assert(grp->getNumChildren() == N_CHILDREN);
	
	return oldData.get();
}

void MeshOctree::processBranchGrp(osg::Group* group, const osg::BoundingBoxd& bbox,
		const GraphicData& data) {
	
	assert(group->getNumChildren() == N_CHILDREN);
	
	// i have to choose the correct branch to recurse on
	for (int i = 0; i < N_CHILDREN; ++i) {
		assert(dynamic_cast< osg::Group * >(group->getChild(0)) != NULL);
		osg::Group *nextGrp = static_cast< osg::Group * >(group->getChild(i));
		
		// extract data
		OctreeNodeData *ocData = MeshingUtils::getUserData< OctreeNodeData >(nextGrp);
		if (ocData->isMyCompetence(bbox)) {
			unsigned int pidx = static_cast< unsigned int >(ocData->getType());
			assert(pidx < 2);
			(this->*(PROCESSERS[pidx]))(nextGrp, bbox, data);
			return;
		}
	}
	
	throw std::runtime_error("Branch not found (but should have to)");
}

void MeshOctree::processLeafGrp(osg::Group* grp, const osg::BoundingBoxd&,
		const GraphicData& data) {
	
	// leaf should be inserted into this data
	LeafNodeData *leafData = MeshingUtils::getUserData< LeafNodeData >(grp);
	
	GraphicData::Elm elm = leafData->insertElm(data);
	data.vinfo->setGraphics(GraphicPointer(elm, grp));
	
	grp->setDataVariance(osg::Node::DYNAMIC);
	
	// now check if we have to split current node
	if ((leafData->getSize() > maxLeafSize) && (leafData->getDepth() < maxDepth)) {
		
		osg::ref_ptr< LeafNodeData > oldData = pushGrp(grp);
		
		// i have to re-insert old data & update its references so...
		GraphicData::List::iterator dataIt = oldData->getElements().begin();
		for (; dataIt != oldData->getElements().end(); ++dataIt) {
			osg::BoundingBoxd bbox = dataIt->sbox->asBoundingBox();
			processBranchGrp(grp, bbox, *dataIt);
		}
	}
}

