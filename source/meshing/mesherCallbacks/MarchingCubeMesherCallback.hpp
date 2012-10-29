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

#include <cassert>

#include <osg/Geode>

class MarchingCubeMesherCallback : public LeafNodeCallback {
	
public:
	MarchingCubeMesherCallback() { }
	
	virtual osg::ref_ptr< osg::Node > buildNode(const LeafNodeData &data) {
		assert(data.isDirty());
		
		throw std::runtime_error("Not implemented yet");
	}
	
	
protected:
	virtual ~MarchingCubeMesherCallback() { }
	
};


#endif /* MARCHINGCUBEMESHERCALLBACK_HPP_ */
