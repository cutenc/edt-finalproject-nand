/*
 * LeafNodeCallback.hpp
 *
 *  Created on: 24/ott/2012
 *      Author: socket
 */

#ifndef LEAFNODECALLBACK_HPP_
#define LEAFNODECALLBACK_HPP_

#include <cassert>

#include <boost/assert.hpp>

#include <osg/Geode>
#include <osg/NodeCallback>

#include "LeafNodeData.hpp"
#include "MeshingUtils.hpp"

/**
 * Class used to convert LeafNodeData stored inside a node into a graphic
 * object. This conversion will be performed only each time data has been
 * flagged as dirty by MeshOctree methods.
 * Pay attention that a single istance of this object will be attached
 * to all needed nodes so class attributes may be used to share informations
 * between different nodes.
 */
class LeafNodeCallback : boost::noncopyable, public osg::NodeCallback {
	
public:
	const static unsigned int NODE_IDX = 0;
	
public:
	LeafNodeCallback() { }
	
	virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv ) {
		
		osg::Group *group = node->asGroup();
		
		// checks if current node's data is dirty and if so update it
		LeafNodeData *data = MeshingUtils::getUserData< LeafNodeData >(group);
		
		if (data->isDirty()) {
			/* we have to rebuild mesh based on current leaf infos
			 */
			osg::ref_ptr< osg::Node > childNode;
			if (data->isEmpty()) {
				childNode = new osg::Geode;
			} else {
				childNode = buildNode(*data);
			}
			
			assert(group->getNumChildren() == 1);
			group->setChild(NODE_IDX, childNode.get());
			
			data->clean();
			
			group->setDataVariance(osg::Node::STATIC);
		} // else, do nothing
		
		this->traverse(node, nv);
	}
	
	virtual osg::ref_ptr< osg::Node > buildNode(const LeafNodeData &data) =0;
	
protected:
	virtual ~LeafNodeCallback() { }
};


#endif /* LEAFNODECALLBACK_HPP_ */
