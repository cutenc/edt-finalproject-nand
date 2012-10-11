/*
 * Vertex.hpp
 *
 *  Created on: 11/ott/2012
 *      Author: socket
 */

#ifndef VERTEX_HPP_
#define VERTEX_HPP_

class Vertex {
	
private:
	const Eigen::Vector3d coords;
	
public:
	Vertex(const Eigen::Vector3d &coords);
	
	virtual ~Vertex();
	
	const Eigen::Vector3d &getCoords() const;
	
};

#endif /* VERTEX_HPP_ */
