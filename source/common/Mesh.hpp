/*
 * Mesh.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef MESH_HPP_
#define MESH_HPP_

#include <ostream>

#include <boost/shared_ptr.hpp>

#include <osg/Node>

class Mesh {
	
public:
	typedef boost::shared_ptr< Mesh > Ptr;
	typedef boost::shared_ptr< const Mesh > ConstPtr;
	
private:
	osg::ref_ptr< osg::Node > meshPtr;
	
public:
	Mesh(osg::Node *mesh);
	virtual ~Mesh();
	
	osg::ref_ptr< osg::Node > getMesh() const;
};

#endif /* MESH_HPP_ */
