/*
 * Triangle.cpp
 *
 *  Created on: 17/ott/2012
 *      Author: alberto
 */

#include "Triangle.hpp"

Triangle::Triangle(osg::Vec3f a, osg::Vec3f b, osg::Vec3f c) {

	triangle = osg::ref_ptr<osg::Geometry>(new osg::Geometry);

	osg::ref_ptr<osg::Vec3Array> vertices = osg::ref_ptr<osg::Vec3Array>(new osg::Vec3Array);
	vertices->push_back(a);    /*       /\ c     */
	vertices->push_back(b);    /*      /  \      */
	vertices->push_back(c);    /*   a /____\ b   */

	triangle->setVertexArray(vertices);

	osg::ref_ptr<osg::DrawElementsUInt> face =
		osg::ref_ptr<osg::DrawElementsUInt>(new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 3));
	face->push_back(1);
	face->push_back(2);  // b -> c -> a
	face->push_back(0);

	triangle->addPrimitiveSet(face);

	osg::ref_ptr<osg::Vec3Array> normalArray = osg::ref_ptr<osg::Vec3Array>(new osg::Vec3Array);
	normalArray->push_back(osg::Vec3(0, -1, 0));

	triangle->setNormalArray(normalArray);

}

Triangle::~Triangle() {
	// TODO Auto-generated destructor stub
}

}
