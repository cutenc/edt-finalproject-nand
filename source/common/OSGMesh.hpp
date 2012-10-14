/*
 * OSGMesh.hpp
 *
 *  Created on: 13/ott/2012
 *      Author: alberto
 */

#ifndef OSGMESH_HPP_
#define OSGMESH_HPP_

class OSGMesh : public Mesh {
public:
	OSGMesh(osg::ref_ptr<osg::Geode> g){
		geode = g;
	}
	virtual ~OSGMesh();

	osg::ref_ptr<osg::Geode> geode;
};

#endif /* OSGMESH_HPP_ */
