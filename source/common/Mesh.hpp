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

class Mesh {
	
public:
	typedef boost::shared_ptr< Mesh > Ptr;
	typedef boost::shared_ptr< const Mesh > ConstPtr;
	
public:
	Mesh() { }
	virtual ~Mesh() { }
	
	virtual std::ostream & toOutStream(std::ostream &os) const =0;
		
	friend std::ostream & operator<<(std::ostream &os, const Mesh &c) {
		return c.toOutStream(os);
	}
};

#endif /* MESH_HPP_ */
