/*
 * Model3D.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef MODEL3D_HPP_
#define MODEL3D_HPP_

class Model3D {

public:
	/* Model3D(Mesh mesh) : mesh(mesh) { } */
	virtual ~Model3D() { }
	virtual Mesh getMeshing() =0;
};

#endif /* MODEL3D_HPP_ */
