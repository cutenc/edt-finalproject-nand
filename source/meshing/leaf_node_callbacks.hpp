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
#include <osg/Array>

#include "milling/Corner.hpp"
#include "common/Utilities.hpp"
#include "visualizer/VisualizationUtils.hpp"

class BoxMesherCallback : public LeafNodeCallback {
	
	const osg::ref_ptr< osg::Vec3Array > normalArray;
	const osg::ref_ptr< osg::Vec4Array > colorArray;
	
public:
	BoxMesherCallback() :
		normalArray(new osg::Vec3Array), colorArray(new osg::Vec4Array)
	{
		
		/* build normal and color array: they will be shared among all builded
		 * objects
		 */
		normalArray->push_back(osg::Vec3(-1, 0, 0));
		normalArray->push_back(osg::Vec3(0, -1, 0));
		normalArray->push_back(osg::Vec3(0, 0, -1));
		normalArray->push_back(osg::Vec3(+1, 0, 0));
		normalArray->push_back(osg::Vec3(0, +1, 0));
		normalArray->push_back(osg::Vec3(0, 0, +1));
		
		colorArray->push_back(osg::Vec4(1, 0, 0, 1));
		colorArray->push_back(osg::Vec4(0, 1, 0, 1));
		colorArray->push_back(osg::Vec4(0, 0, 1, 1));
		colorArray->push_back(osg::Vec4(.5, .5, 0, 1));
		colorArray->push_back(osg::Vec4(0, .5, .5, 1));
		colorArray->push_back(osg::Vec4(.5, 0, .5, 1));
		
	}
	
	virtual osg::ref_ptr< osg::Node > buildNode(const LeafNodeData &data) {
		if (data.isEmpty()) {
			return new osg::Geode;
		}
		
		assert(data.isDirty());
		
		osg::ref_ptr< osg::Geometry > geom = new osg::Geometry;
		u_int nElm = 0, totElm = data.getElements().size();
		
		/* face order:
		 * left, bottom, front, rigth, rear, top
		 */
		
		// build normal & its indices
		geom->setNormalArray(normalArray.get());
		geom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);
		osg::UByteArray *normalIndexArray = new osg::UByteArray;
		geom->setNormalIndices(normalIndexArray);
		
		// build colors & its indices
		geom->setColorArray(colorArray.get());
		geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
		osg::UByteArray *colorIndexArray = new osg::UByteArray;
		geom->setColorIndices(colorIndexArray);
		
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
			
			/* 
			 * BUILD FACES
			 */
			osg::ref_ptr< osg::DrawElementsUInt > faces = 
					new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS);
			
			// left face
			faces->push_back(0 + shift);
			faces->push_back(4 + shift);
			faces->push_back(7 + shift);
			faces->push_back(3 + shift);
			
			// bottom face
			faces->push_back(3 + shift);
			faces->push_back(2 + shift);
			faces->push_back(1 + shift);
			faces->push_back(0 + shift);
			
			// front face
			faces->push_back(1 + shift);
			faces->push_back(5 + shift);
			faces->push_back(4 + shift);
			faces->push_back(0 + shift);
			
			// rigth face
			faces->push_back(2 + shift);
			faces->push_back(6 + shift);
			faces->push_back(5 + shift);
			faces->push_back(1 + shift);
			
			// rear face
			faces->push_back(2 + shift);
			faces->push_back(6 + shift);
			faces->push_back(7 + shift);
			faces->push_back(3 + shift);
			
			// top face
			faces->push_back(5 + shift);
			faces->push_back(6 + shift);
			faces->push_back(7 + shift);
			faces->push_back(4 + shift);
			
			geom->addPrimitiveSet(faces);
			
			
			/*
			 * BUILD NORMAL ARRAY INDEX
			 */
			for (u_char i = 0; i < 6; ++i) {
				// normals are always 0, 1, 2, ..., 5
				normalIndexArray->push_back(i);
			}
			
			/*
			 * BUILD COLOR ARRAY INDEX
			 */
			for (u_char i = 0; i < 6; ++i) {
				// normals are always 0, 1, 2, ..., 5
				colorIndexArray->push_back(i);
			}
			
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
