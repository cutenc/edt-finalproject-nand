/*
 * VisualizationUtils.cpp
 *
 *  Created on: 20/ott/2012
 *      Author: socket
 */

#include "VisualizationUtils.hpp"

#include <osg/LineWidth>
#include <osg/StateSet>

#include "common/constants.hpp"

VisualizationUtils::VisualizationUtils() {
}

VisualizationUtils::~VisualizationUtils() {
}

osg::ref_ptr<osg::ShapeDrawable> VisualizationUtils::buildCylinder(
		const osg::Vec3& center, float radius, float height) {
	
	osg::ref_ptr< osg::Cylinder > cyl = new osg::Cylinder(center, radius, height);
	osg::ref_ptr< osg::ShapeDrawable > sdraw = new osg::ShapeDrawable(cyl.get());
	
	return sdraw.get();
}

osg::ref_ptr<osg::ShapeDrawable> VisualizationUtils::buildCylinder(float radius,
		float height) {
	return buildCylinder(osg::Vec3(), radius, height);
}

osg::ref_ptr<osg::ShapeDrawable> VisualizationUtils::buildBox(
		const osg::Vec3& center, float x, float y, float z) {
	
	osg::ref_ptr< osg::Box > box = new osg::Box(center, x, y, z);
	osg::ref_ptr< osg::ShapeDrawable > sdraw = new osg::ShapeDrawable(box.get());
	
	return sdraw.get();	
	
}

osg::ref_ptr<osg::ShapeDrawable> VisualizationUtils::buildBox(float x, float y,
		float z) {
	return buildBox(osg::Vec3(), x, y, z);
}

osg::ref_ptr< osg::Geometry > VisualizationUtils::getAxis() {
	const static osg::ref_ptr< osg::Geometry > AXIS = buildAxis();
	
	return AXIS.get();
}

osg::ref_ptr<osg::Geometry> VisualizationUtils::buildAxis() {
	
	// set up the Geometry
	osg::ref_ptr< osg::Geometry > geom = new osg::Geometry;
	
	// create the coords array used by the LINE primitive (start & end points)
	osg::Vec3Array* coords = new osg::Vec3Array;
	geom->setVertexArray(coords);
	
	for (int i = 0; i < 6; ++i) {
		coords->push_back( osg::Vec3() );
	}
	for (int i = 0; i < 3; ++i) {
		(*coords)[i*2+1][i] += AXIS_LENGTH;
	}
	
	// colors are expressed in RGB-Opacity format
	osg::Vec4Array* color = new osg::Vec4Array;
	color->push_back( osg::Vec4(1, 0, 0, 1) );
	color->push_back( osg::Vec4(0, 1, 0, 1) );
	color->push_back( osg::Vec4(0, 0, 1, 1) );
	
	geom->setColorArray(color);
	geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	
	geom->addPrimitiveSet(
			new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 6)
	);
	
	osg::StateSet* stateset = new osg::StateSet;
	osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth(4.0f);
	stateset->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geom->setStateSet(stateset);
	
	return geom.get();
}


//osg::Drawable* createSquare(const osg::Vec3& corner,const osg::Vec3& width,const osg::Vec3& height, osg::Image* image=NULL)
//{
//    // set up the Geometry.
//    osg::Geometry* geom = new osg::Geometry;
//
//    osg::Vec3Array* coords = new osg::Vec3Array(4);
//    (*coords)[0] = corner;
//    (*coords)[1] = corner+width;
//    (*coords)[2] = corner+width+height;
//    (*coords)[3] = corner+height;
//
//
//    geom->setVertexArray(coords);
//
//    osg::Vec3Array* norms = new osg::Vec3Array(1);
//    (*norms)[0] = width^height;
//    (*norms)[0].normalize();
//
//    geom->setNormalArray(norms);
//    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
//
//    osg::Vec2Array* tcoords = new osg::Vec2Array(4);
//    (*tcoords)[0].set(0.0f,0.0f);
//    (*tcoords)[1].set(1.0f,0.0f);
//    (*tcoords)[2].set(1.0f,1.0f);
//    (*tcoords)[3].set(0.0f,1.0f);
//    geom->setTexCoordArray(0,tcoords);
//
//    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
//
//    if (image)
//    {
//        osg::StateSet* stateset = new osg::StateSet;
//        osg::Texture2D* texture = new osg::Texture2D;
//        texture->setImage(image);
//        stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
//        geom->setStateSet(stateset);
//    }
//
//    return geom;
//}


//		static osg::ref_ptr<osg::Geometry> createCube() {
//			osg::ref_ptr<osg::Geometry> cubeGeometry = osg::ref_ptr<osg::Geometry>(new osg::Geometry);
//
//			// Specify the vertices:
//			osg::ref_ptr<osg::Vec3Array> cubeVertices = osg::ref_ptr<osg::Vec3Array>(new osg::Vec3Array);
//			cubeVertices->push_back( osg::Vec3(0, 0, 0) );
//			cubeVertices->push_back( osg::Vec3(0, 0, 1) );
//			cubeVertices->push_back( osg::Vec3(0, 1, 0) );
//			cubeVertices->push_back( osg::Vec3(0, 1, 1) );
//			cubeVertices->push_back( osg::Vec3(1, 0, 0) );
//			cubeVertices->push_back( osg::Vec3(1, 0, 1) );
//			cubeVertices->push_back( osg::Vec3(1, 1, 0) );
//			cubeVertices->push_back( osg::Vec3(1, 1, 1) );
//
//
//			// Associate this set of vertices with the geometry associated with the
//			// geode we added to the scene.
//			cubeGeometry->setVertexArray( cubeVertices );
//
//			osg::ref_ptr<osg::DrawElementsUInt> cubeFace1 =
//				osg::ref_ptr<osg::DrawElementsUInt>(new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4));
//			cubeFace1->push_back(2);
//			cubeFace1->push_back(3);
//			cubeFace1->push_back(1);
//			cubeFace1->push_back(0);
//			cubeGeometry->addPrimitiveSet(cubeFace1);
//	//		cubeGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS));
//
//			osg::ref_ptr<osg::DrawElementsUInt> cubeFace2 =
//				osg::ref_ptr<osg::DrawElementsUInt>(new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4));
//			cubeFace2->push_back(1);
//			cubeFace2->push_back(5);
//			cubeFace2->push_back(4);
//			cubeFace2->push_back(0);
//			cubeGeometry->addPrimitiveSet(cubeFace2);
//	//		cubeGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS));
//
//		//
//			osg::ref_ptr<osg::DrawElementsUInt> cubeFace3 =
//				osg::ref_ptr<osg::DrawElementsUInt>(new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4));
//			cubeFace3->push_back(4);
//			cubeFace3->push_back(6);
//			cubeFace3->push_back(2);
//			cubeFace3->push_back(0);
//			cubeGeometry->addPrimitiveSet(cubeFace3);
//	//		cubeGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS));
//
//		//
//			osg::ref_ptr<osg::DrawElementsUInt> cubeFace4 =
//				osg::ref_ptr<osg::DrawElementsUInt>(new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4));
//			cubeFace4->push_back(5);
//			cubeFace4->push_back(4);
//			cubeFace4->push_back(6);
//			cubeFace4->push_back(7);
//			cubeGeometry->addPrimitiveSet(cubeFace4);
//	//		cubeGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS));
//
//			osg::ref_ptr<osg::DrawElementsUInt> cubeFace5 =
//				osg::ref_ptr<osg::DrawElementsUInt>(new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4));
//			cubeFace5->push_back(3);
//			cubeFace5->push_back(2);
//			cubeFace5->push_back(6);
//			cubeFace5->push_back(7);
//			cubeGeometry->addPrimitiveSet(cubeFace5);
//	//		cubeGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS));
//
//			osg::ref_ptr<osg::DrawElementsUInt> cubeFace6 =
//				osg::ref_ptr<osg::DrawElementsUInt>(new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4));
//			cubeFace6->push_back(5);
//			cubeFace6->push_back(1);
//			cubeFace6->push_back(3);
//			cubeFace6->push_back(7);
//			cubeGeometry->addPrimitiveSet(cubeFace6);
//	//		cubeGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS));
//
//			osg::ref_ptr<osg::Vec3Array> normalArray = osg::ref_ptr<osg::Vec3Array>(new osg::Vec3Array);
//			normalArray->push_back(osg::Vec3(-1, 0, 0));
//			normalArray->push_back(osg::Vec3(0, -1, 0));
//			normalArray->push_back(osg::Vec3(0, 0, -1));
//			normalArray->push_back(osg::Vec3(+1, 0, 0));
//			normalArray->push_back(osg::Vec3(0, +1, 0));
//			normalArray->push_back(osg::Vec3(0, 0, +1));
//			cubeGeometry->setNormalArray(normalArray);
//
//			return cubeGeometry;
//	}
