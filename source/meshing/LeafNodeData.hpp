/*
 * LeafNodeData.hpp
 *
 *  Created on: 24/ott/2012
 *      Author: socket
 */

#ifndef LEAFNODEDATA_HPP_
#define LEAFNODEDATA_HPP_

#include <utility>
#include <list>

#include "OctreeNodeData.hpp"
#include "milling/graphics_info.hpp"

class LeafNodeData: public OctreeNodeData {
	
	
private:
	GraphicData::List elements;
	
	/**
	 * Stores the number of elements contained in the list. This variable is
	 * needed because <tt>list.size()</tt> may be O(N) with N number of
	 * contained elements. This is an implementation dependant detail but
	 * an O(1) <tt>list.splice</tt> implementation necessarily
	 * cause <tt>list.size</tt> to be O(N).
	 */
	unsigned int storedElms;
	
	bool dirty;
	
public:
	LeafNodeData(const osg::BoundingBoxd &bbox, unsigned char depth);
	
	virtual NodeDataType getType() const;
	
	GraphicData::Elm insertElm(const GraphicData &info);
	void deleteElm(const GraphicData::Elm &ref);
	void updateElm(const GraphicData::Elm &ref, const GraphicData &info);
	unsigned int getSize() const;
	
	const GraphicData::List &getElements() const;
	GraphicData::List &getElements();
	
	bool isDirty() const;
	void clean();
	
	bool isEmpty() const;
	
private:
	void setDirty();
	
protected:
	virtual ~LeafNodeData();
};

#endif /* LEAFNODEDATA_HPP_ */
