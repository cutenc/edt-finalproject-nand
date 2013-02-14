/**
 * BranchNodeData.hpp
 *
 *  Created on: 24/ott/2012
 *      Author: socket
 */

#ifndef BRANCHNODEDATA_HPP_
#define BRANCHNODEDATA_HPP_

#include "OctreeNodeData.hpp"

/**
 * @class BranchNodeData
 *
 * describes an internal node of the octree
 */
class BranchNodeData: public OctreeNodeData {
	
public:
	/**
	 * constructor
	 *
	 * @param bbox
	 * @param depth
	 */
	BranchNodeData(const osg::BoundingBoxd &bbox, unsigned char depth) :
		OctreeNodeData(bbox, depth) { }
	
	/**
	 *
	 * @return the type of the ndoe
	 */
	virtual NodeDataType getType() const {
		return OctreeNodeData::BranchLODData;
	}
	
protected:
	virtual ~BranchNodeData() { }
};

#endif /* BRANCHNODEDATA_HPP_ */
