/**
 * MeshOctree.hpp
 *
 *  Created on: 24/ott/2012
 *      Author: socket
 */

#ifndef MESHOCTREE_HPP_
#define MESHOCTREE_HPP_

#include <utility>

#include <osg/LOD>
#include <osg/BoundingBox>

#include "milling/graphics_info.hpp"
#include "LeafNodeData.hpp"
#include "LeafNodeCallback.hpp"

/**
 * @class MeshOctree
 *
 * the octree containing the mesh to be visualized
 */
class MeshOctree {
	
private:
	typedef void (MeshOctree::* NodeDataProcessers)(osg::Group *lod, const osg::BoundingBoxd &, const GraphicData &);
	
private:
	static const unsigned char N_CHILDREN = 8;
	
	static const unsigned int GROUP_IDX = 0;
	
	const osg::ref_ptr< LeafNodeCallback > groupCallback;
	const unsigned int maxLeafSize, maxDepth;
	NodeDataProcessers PROCESSERS[2];
	osg::ref_ptr< osg::Group > ROOT;
	
public:
	/**
	 * constructor
	 *
	 * @param bbox
	 * @param nodeCallback
	 * @param dataPerLeaf
	 * @param maxDepth
	 */
	MeshOctree(const osg::BoundingBoxd &bbox, LeafNodeCallback *nodeCallback, 
			unsigned int dataPerLeaf, unsigned int maxDepth);
	
	virtual ~MeshOctree();
	
	/**
	 * insert a node
	 * 
	 * @param gdata
	 * @return \c true if added, \c false otherwise
	 */
	bool addData(const GraphicData &gdata);

	/**
	 * updates a node
	 * @param ref
	 * @param gdata
	 */
	void updateData(const GraphicPointer &ref, const GraphicData &gdata);

	/**
	 * delete a node
	 * @param ref
	 */
	void removeData(const GraphicPointer &ref);
	
	/**
	 *
	 * @return the root of the scene (oc)tree
	 */
	osg::Group * getRoot();
	
private:
	osg::ref_ptr< osg::Group > createLeafGrp(const osg::BoundingBoxd &bbox, unsigned char depth);
	osg::ref_ptr< LeafNodeData > pushGrp(osg::Group *lod);
	
	void processBranchGrp(osg::Group *lod, const osg::BoundingBoxd &bbox, const GraphicData &data);
	void processLeafGrp(osg::Group *lod, const osg::BoundingBoxd &bbox, const GraphicData &data);
	
};

#endif /* MESHOCTREE_HPP_ */
