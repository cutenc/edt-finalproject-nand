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
#include "meshing/Face.hpp"

class BoxMesherCallback : public LeafNodeCallback {
	
	const osg::ref_ptr< osg::Vec3Array > normalArray;
	const osg::ref_ptr< osg::Vec4Array > colorArray;
	
	const Eigen::Vector3d STOCK_HALF_EXTENTS;
	
public:
	BoxMesherCallback(const StockDescription& desc) :
		normalArray(new osg::Vec3Array(Face::N_FACES)), colorArray(new osg::Vec4Array(Face::N_FACES)),
		STOCK_HALF_EXTENTS(desc.getGeometry()->asEigen() * 0.5)
	{
		
		/* build normal and color array: they will be shared among all builded
		 * objects
		 */
		(*normalArray)[Face::LEFT] = osg::Vec3(-1, 0, 0);
		(*normalArray)[Face::FRONT] = osg::Vec3(0, -1, 0);
		(*normalArray)[Face::BOTTOM] = osg::Vec3(0, 0, -1);
		(*normalArray)[Face::RIGHT] = osg::Vec3(+1, 0, 0);
		(*normalArray)[Face::REAR] = osg::Vec3(0, +1, 0);
		(*normalArray)[Face::TOP] = osg::Vec3(0, 0, +1);
		
		(*colorArray)[Face::LEFT] = osg::Vec4(1, 0, 0, 1);
		(*colorArray)[Face::FRONT] = osg::Vec4(0, 1, 0, 1);
		(*colorArray)[Face::BOTTOM] = osg::Vec4(0, 0, 1, 1);
		(*colorArray)[Face::RIGHT] = osg::Vec4(.5, .5, 0, 1);
		(*colorArray)[Face::REAR] = osg::Vec4(0, .5, .5, 1);
		(*colorArray)[Face::TOP] = osg::Vec4(.5, 0, .5, 1);
		
	}
	
	virtual osg::ref_ptr< osg::Node > buildNode(const LeafNodeData &data) {
		assert(data.isDirty() && !data.isEmpty());
		
		osg::Geometry *geom = new osg::Geometry;
		
		/* face order:
		 * left, front, bottom, rigth, rear, top
		 */
		
		// build normal & its indices
		geom->setNormalArray(normalArray.get());
		osg::UByteArray *normalIndexArray = new osg::UByteArray;
		geom->setNormalIndices(normalIndexArray);
		geom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);
		
		// build colors & its indices
		geom->setColorArray(colorArray.get());
		osg::UByteArray *colorIndexArray = new osg::UByteArray;
		geom->setColorIndices(colorIndexArray);
		geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
		
		// build vertices array
		osg::Vec3Array *vertices = new osg::Vec3Array;
		geom->setVertexArray(vertices);
		
		/* 
		 * BUILD FACES
		 */
		GraphicData::List::const_iterator dataIt = data.getElements().begin();
		for (; dataIt != data.getElements().end(); ++dataIt) {
			
			FaceIterator fit = FaceIterator::begin();
			for (; fit != FaceIterator::end(); ++fit) {
				
				if (! (Face::isBorderFace(*fit, STOCK_HALF_EXTENTS, *dataIt->sbox)
						|| dataIt->vinfo->isIntersecting()
					)) {
					continue;
				}
				
				// build current face
				for (int i = 0; i < 4; ++i) {
					vertices->push_back(
						GeometryUtils::toOsg(
							dataIt->sbox->getCorner(Face::FACE_ADJACENCY[*fit][i])
						)
					);
				}
				
				// add proper normal & color
				normalIndexArray->push_back(*fit);
				colorIndexArray->push_back(*fit);
			}
		}
		
		assert(vertices->size() % 4 == 0);
		
		osg::DrawArrays *faces = new osg::DrawArrays(
				osg::PrimitiveSet::QUADS,
				0,
				vertices->size()
		);
		geom->addPrimitiveSet(faces);
		
		osg::ref_ptr< osg::Geode > geode = new osg::Geode;
		geode->addDrawable(geom);
		
		return geode.get();
	}
	
protected:
	virtual ~BoxMesherCallback() { }
	
};


#endif /* BOXMESHERCALLBACK_HPP_ */
