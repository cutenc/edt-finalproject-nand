/*
 * LeafNodeData.cpp
 *
 *  Created on: 24/ott/2012
 *      Author: socket
 */

#include "LeafNodeData.hpp"

#include <cassert>

LeafNodeData::LeafNodeData(const osg::BoundingBoxd& bbox, u_char depth) :
	OctreeNodeData(bbox, depth), storedElms(0), dirty(true) 
{ }

OctreeNodeData::NodeDataType LeafNodeData::getType() const {
	return OctreeNodeData::LeafLODData;
}

GraphicData::Elm LeafNodeData::insertElm(const GraphicData& info) {
	setDirty(); storedElms++;
	return elements.insert(elements.end(), info);
}

void LeafNodeData::deleteElm(const GraphicData::Elm& ref) {
	setDirty(); storedElms--;
	elements.erase(ref);
}

void LeafNodeData::updateElm(const GraphicData::Elm& ref,
		const GraphicData& info) {
	setDirty();
	*ref = info;
}

u_int LeafNodeData::getSize() const {
	// elements.size() may take constant time (in libc it is so)
	return storedElms;
}

const GraphicData::List& LeafNodeData::getElements() const {
	return this->elements;
}

GraphicData::List& LeafNodeData::getElements() {
	return this->elements;
}

bool LeafNodeData::isDirty() const {
	return this->dirty;
}

void LeafNodeData::clean() {
	assert(isDirty());
	this->dirty = false;
}

bool LeafNodeData::isEmpty() const {
	assert(this->elements.empty() == (storedElms == 0));
	return storedElms == 0;
}

void LeafNodeData::setDirty() {
	this->dirty = true;
}

LeafNodeData::~LeafNodeData() {
}
