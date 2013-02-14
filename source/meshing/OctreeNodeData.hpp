/**
 * OctreeNodeData.hpp
 *
 *  Created on: 24/ott/2012
 *      Author: socket
 */

#ifndef OCTREENODEDATA_HPP_
#define OCTREENODEDATA_HPP_

#include <osg/Referenced>
#include <osg/BoundingBox>

/**
 * @class OctreeNodeData
 *
 * data of the octree nodes
 */
class OctreeNodeData: public osg::Referenced {
	
public:
	enum NodeDataType {
		BranchLODData = 0,
		LeafLODData = 1
	};
	
private:
	const osg::BoundingBoxd bbox;
	const unsigned char depth;
	
public:
	/**
	 * constructor
	 * @param bbox the data
	 * @param depth the level at which the data is
	 */
	OctreeNodeData(const osg::BoundingBoxd &bbox, unsigned char depth);
	
	/**
	 *
	 * @return the bounding box of the node
	 */
	const osg::BoundingBoxd &getCompetenceBox() const;
	
	/**
	 * 
	 * @param bbox
	 * @return \c true if given box is contained in this one or at least it
	 * is intersecting but with box.center contained in this box
	 */
	bool isMyCompetence(const osg::BoundingBoxd &bbox);

	/**
	 *
	 * @return the depth of the node
	 */
	unsigned char getDepth() const;
	
	/**
	 *
	 * @return the data type of the node
	 */
	virtual NodeDataType getType() const =0;
	
protected:
	virtual ~OctreeNodeData();
};

#endif /* OCTREENODEDATA_HPP_ */
