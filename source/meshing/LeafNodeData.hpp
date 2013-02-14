/**
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

/**
 * @class LeafNodeData
 *
 * contains the data of a leaf of the scene tree.
 */
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
	/**
	 * constructor
	 *
	 * @param bbox the data
	 * @param depth the level of the tree the data is
	 */
	LeafNodeData(const osg::BoundingBoxd &bbox, unsigned char depth);
	
	/**
	 *
	 * @return the type of the node
	 */
	virtual NodeDataType getType() const;
	
	/**
	 * insert the given element
	 *
	 * @param info
	 * @return
	 */
	GraphicData::Elm insertElm(const GraphicData &info);

	/**
	 * delete specified element
	 *
	 * @param ref
	 */
	void deleteElm(const GraphicData::Elm &ref);

	/**
	 * update specified element
	 *
	 * @param ref
	 * @param info
	 */
	void updateElm(const GraphicData::Elm &ref, const GraphicData &info);

	/**
	 *
	 * @return the size of the data
	 */
	unsigned int getSize() const;
	
	/**
	 *
	 * @return list of elements containing the leaf data
	 */
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
