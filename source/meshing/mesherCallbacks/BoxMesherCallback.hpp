/*
 * BoxMesherCallback.hpp
 *
 *  Created on: 29/ott/2012
 *      Author: socket
 *      
 * DO NOT INCLUDE THIS FILE DIRECTLY, YOU SHOULD USE
 * "leaf_node_callbacks.hpp"
 */

#ifndef BOXMESHERCALLBACK_HPP_
#define BOXMESHERCALLBACK_HPP_

#include "meshing/LeafNodeCallback.hpp"

#include <cassert>

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Array>

#include "milling/Corner.hpp"
#include "common/Utilities.hpp"

class BoxMesherCallback : public LeafNodeCallback {
	
	const osg::ref_ptr< osg::Vec3Array > normalArray;
	const osg::ref_ptr< osg::Vec4Array > colorArray;
	const osg::ref_ptr< osg::UByteArray > normalIndexArray;
	const osg::ref_ptr< osg::UByteArray > colorIndexArray;
	
public:
	BoxMesherCallback() :
		normalArray(new osg::Vec3Array), colorArray(new osg::Vec4Array),
		normalIndexArray(new osg::UByteArray), colorIndexArray(new osg::UByteArray)
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
		assert(data.isDirty() && !data.isEmpty());
		
		osg::Geometry *geom = new osg::Geometry;
		unsigned int nElm = 0, totElm = data.getElements().size();
		
		ensureEnoughNormalsAndColors(totElm);
		
		/* face order:
		 * left, front, bottom, rigth, rear, top
		 */
		
		// build normal & its indices
		geom->setNormalArray(normalArray.get());
		geom->setNormalIndices(normalIndexArray.get());
		geom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);
		
		// build colors & its indices
		geom->setColorArray(colorArray.get());
		geom->setColorIndices(colorIndexArray.get());
		geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
		
		// build vertices array
		osg::Vec3Array *vertices = new osg::Vec3Array(Corner::N_CORNERS * totElm);
		geom->setVertexArray(vertices);
		
		/* 
		 * BUILD FACES
		 */
		osg::DrawElementsUInt *faces = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS);
		
		GraphicData::List::const_iterator dataIt = data.getElements().begin();
		for (; dataIt != data.getElements().end(); ++dataIt) {
			
			unsigned int shift = nElm * Corner::N_CORNERS;
			
			CornerIterator cit = CornerIterator::begin();
			for (; cit != CornerIterator::end(); ++cit) {
				(*vertices)[*cit + shift] = GeometryUtils::toOsg(dataIt->sbox->getCorner(*cit));
			}
			
			// left face
			faces->push_back(0 + shift);
			faces->push_back(4 + shift);
			faces->push_back(7 + shift);
			faces->push_back(3 + shift);
			
			// front face
			faces->push_back(1 + shift);
			faces->push_back(5 + shift);
			faces->push_back(4 + shift);
			faces->push_back(0 + shift);
			
			// bottom face
			faces->push_back(3 + shift);
			faces->push_back(2 + shift);
			faces->push_back(1 + shift);
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
			
			++nElm;
		}

		geom->addPrimitiveSet(faces);
		
		osg::ref_ptr< osg::Geode > geode = new osg::Geode;
		geode->addDrawable(geom);
		
		return geode.get();
	}
	
protected:
	virtual ~BoxMesherCallback() { }
	
private:
	void ensureEnoughNormalsAndColors(unsigned int totElms) {
		assert(normalIndexArray->size() == colorIndexArray->size());
		assert(normalIndexArray->size() % 6 == 0);
		
		unsigned int availElms = normalIndexArray->size() / 6;
		if (totElms <= availElms) {
			return;
		}
		
		for (unsigned int count = availElms; count < totElms; ++count) {
			for (unsigned char i = 0; i < 6; ++i) {
				// normals are always 0, 1, 2, ..., 5
				normalIndexArray->push_back(i);
				// colors are always 0, 1, 2, ..., 5
				colorIndexArray->push_back(i);
			}
		}
		
	}
	
};


#endif /* BOXMESHERCALLBACK_HPP_ */
