/*
 * Model3D.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef MODEL3D_HPP_
#define MODEL3D_HPP_

#include "Mesh.hpp"

class Model3D {

public:
	virtual ~Model3D() { }
	
	virtual Mesh::Ptr getMeshing() =0;
	
};

#endif /* MODEL3D_HPP_ */
