/*
 * graphics_info.hpp
 *
 *  Created on: 25/ott/2012
 *      Author: socket
 */

#ifndef GRAPHICS_INFO_HPP_
#define GRAPHICS_INFO_HPP_

#include <list>

#include <boost/shared_ptr.hpp>

#include <osg/LOD>

#include "ShiftedBox.hpp"

class VoxelInfo;

struct GraphicData {
	typedef std::list< GraphicData > List;
	typedef List::iterator Elm;
	typedef boost::shared_ptr< VoxelInfo > VoxelInfoPtr;
	
	
	GraphicData(const ShiftedBox::ConstPtr &sbox, const VoxelInfoPtr &vinfo) :
		sbox(sbox), vinfo(vinfo) { }
	virtual ~GraphicData() { }
	
	ShiftedBox::ConstPtr sbox;
	VoxelInfoPtr vinfo;
};

struct GraphicPointer {
	GraphicPointer() { }
	GraphicPointer(const GraphicData::Elm &item, const osg::ref_ptr< osg::Node > &node) :
		item(item), node(node) { }
	virtual ~GraphicPointer() { }
	
	GraphicData::Elm item;
	osg::ref_ptr< osg::Node > node;
};


#endif /* GRAPHICS_INFO_HPP_ */
