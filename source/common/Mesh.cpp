/*
 * Mesh.cpp
 *
 *  Created on: 21/ott/2012
 *      Author: socket
 */

#include "Mesh.hpp"

Mesh::Mesh(osg::Node* mesh) : meshPtr(mesh) 
{ }

Mesh::~Mesh() {
}

osg::ref_ptr<osg::Node> Mesh::getMesh() const {
	return meshPtr.get();
}
