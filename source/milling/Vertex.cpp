/*
 * Vertex.cpp
 *
 *  Created on: 11/ott/2012
 *      Author: socket
 */

#include "Vertex.hpp"


Vertex::Vertex(const Eigen::Vector3d& coords) : coords(coords)
{ }

Vertex::~Vertex() {
}

const Eigen::Vector3d& Vertex::getCoords() const {
	return this->coords;
}
