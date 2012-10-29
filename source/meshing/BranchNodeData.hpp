/*
 * BranchNodeData.hpp
 *
 *  Created on: 24/ott/2012
 *      Author: socket
 */

#ifndef BRANCHNODEDATA_HPP_
#define BRANCHNODEDATA_HPP_

#include "OctreeNodeData.hpp"

class BranchNodeData: public OctreeNodeData {
	
public:
	BranchNodeData(const osg::BoundingBoxd &bbox, u_char depth) :
		OctreeNodeData(bbox, depth) { }
	
	virtual NodeDataType getType() const {
		return OctreeNodeData::BranchLODData;
	}
	
protected:
	virtual ~BranchNodeData() { }
};

#endif /* BRANCHNODEDATA_HPP_ */
