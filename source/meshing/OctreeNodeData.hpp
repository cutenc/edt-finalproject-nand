/*
 * OctreeNodeData.hpp
 *
 *  Created on: 24/ott/2012
 *      Author: socket
 */

#ifndef OCTREENODEDATA_HPP_
#define OCTREENODEDATA_HPP_

#include <osg/Referenced>
#include <osg/BoundingBox>

class OctreeNodeData: public osg::Referenced {
	
public:
	typedef unsigned char u_char;
	enum NodeDataType {
		BranchLODData = 0,
		LeafLODData = 1
	};
	
private:
	const osg::BoundingBoxd bbox;
	const u_char depth;
	
public:
	OctreeNodeData(const osg::BoundingBoxd &bbox, u_char depth);
	
	const osg::BoundingBoxd &getCompetenceBox() const;
	
	/**
	 * 
	 * @param bbox
	 * @return \c true if given box is contained in this one or at least it
	 * is intersecting but with box.center contained in this box
	 */
	bool isMyCompetence(const osg::BoundingBoxd &bbox);
	u_char getDepth() const;
	
	virtual NodeDataType getType() const =0;
	
protected:
	virtual ~OctreeNodeData();
};

#endif /* OCTREENODEDATA_HPP_ */
