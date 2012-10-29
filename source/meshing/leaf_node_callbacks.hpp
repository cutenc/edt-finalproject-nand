/*
 * leaf_node_callbacks.hpp
 *
 *  Created on: 26/ott/2012
 *      Author: socket
 */

#ifndef LEAF_NODE_CALLBACKS_HPP_
#define LEAF_NODE_CALLBACKS_HPP_

#include "LeafNodeCallback.hpp"

#include <cassert>

#include <osg/Geometry>
#include <osg/Geode>

#include "milling/Corner.hpp"
#include "common/Utilities.hpp"
#include "visualizer/VisualizationUtils.hpp"

class BoxMesherCallback : public LeafNodeCallback {
	
public:
	BoxMesherCallback() { }
	
	virtual osg::ref_ptr< osg::Node > buildNode(const LeafNodeData &data) {
		assert(data.isDirty());
		
		osg::ref_ptr< osg::Geometry > geom = new osg::Geometry;
		u_int nElm = 0, totElm = data.getElements().size();
		
		/* face order:
		 * left, bottom, front, rigth, rear, top
		 */
		
		// build normal
		osg::ref_ptr< osg::Vec3Array > normalArray = new osg::Vec3Array;
		normalArray->push_back(osg::Vec3(-1, 0, 0));
		normalArray->push_back(osg::Vec3(0, -1, 0));
		normalArray->push_back(osg::Vec3(0, 0, -1));
		normalArray->push_back(osg::Vec3(+1, 0, 0));
		normalArray->push_back(osg::Vec3(0, +1, 0));
		normalArray->push_back(osg::Vec3(0, 0, +1));
		geom->setNormalArray(normalArray.get());
		geom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
		
		// build colors
		osg::ref_ptr< osg::Vec4Array > colorArray = new osg::Vec4Array;
		colorArray->push_back(osg::Vec4(1, 0, 0, 1));
		colorArray->push_back(osg::Vec4(0, 1, 0, 1));
		colorArray->push_back(osg::Vec4(0, 0, 1, 1));
		colorArray->push_back(osg::Vec4(.5, .5, 0, 1));
		colorArray->push_back(osg::Vec4(0, .5, .5, 1));
		colorArray->push_back(osg::Vec4(.5, 0, .5, 1));
		geom->setColorArray(colorArray.get());
		geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
		
		// build vertices
		osg::ref_ptr< osg::Vec3Array > vertices = new osg::Vec3Array(Corner::N_CORNERS * totElm);
		geom->setVertexArray(vertices.get());
		
		GraphicData::List::const_iterator dataIt = data.getElements().begin();
		for (; dataIt != data.getElements().end(); ++dataIt) {
			
			u_int shift = nElm * Corner::N_CORNERS;
			
			CornerIterator cit = CornerIterator::begin();
			for (; cit != CornerIterator::end(); ++cit) {
				(*vertices)[*cit + shift] = GeometryUtils::toOsg(dataIt->sbox->getCorner(*cit));
			}
			
			osg::ref_ptr< osg::DrawElementsUInt > faceLeft = 
					new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4);
			faceLeft->push_back(0 + shift);
			faceLeft->push_back(4 + shift);
			faceLeft->push_back(7 + shift);
			faceLeft->push_back(3 + shift);
			geom->addPrimitiveSet(faceLeft);
			
			osg::ref_ptr< osg::DrawElementsUInt > faceBottom = 
					new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4);
			faceBottom->push_back(3 + shift);
			faceBottom->push_back(2 + shift);
			faceBottom->push_back(1 + shift);
			faceBottom->push_back(0 + shift);
			geom->addPrimitiveSet(faceBottom);
			
			osg::ref_ptr< osg::DrawElementsUInt > faceFront = 
					new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4);
			faceFront->push_back(1 + shift);
			faceFront->push_back(5 + shift);
			faceFront->push_back(4 + shift);
			faceFront->push_back(0 + shift);
			geom->addPrimitiveSet(faceFront);
			
			osg::ref_ptr< osg::DrawElementsUInt > faceRight = 
					new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4);
			faceRight->push_back(2 + shift);
			faceRight->push_back(6 + shift);
			faceRight->push_back(5 + shift);
			faceRight->push_back(1 + shift);
			geom->addPrimitiveSet(faceRight);
			
			osg::ref_ptr< osg::DrawElementsUInt > faceRear = 
					new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4);
			faceRear->push_back(2 + shift);
			faceRear->push_back(6 + shift);
			faceRear->push_back(7 + shift);
			faceRear->push_back(3 + shift);
			geom->addPrimitiveSet(faceRear);
			
			osg::ref_ptr< osg::DrawElementsUInt > faceTop = 
					new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4);
			faceTop->push_back(5 + shift);
			faceTop->push_back(6 + shift);
			faceTop->push_back(7 + shift);
			faceTop->push_back(4 + shift);
			geom->addPrimitiveSet(faceTop);
			++nElm;
		}
		
		osg::ref_ptr< osg::Geode > geode = new osg::Geode;
		geode->addDrawable(geom.get());
		
		return geode.get();
	}
	
protected:
	virtual ~BoxMesherCallback() { }
	
};


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


#endif /* LEAF_NODE_CALLBACKS_HPP_ */
