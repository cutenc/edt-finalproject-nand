/*
 * Mesher.hpp
 *
 *  Created on: 22/set/2012
 *      Author: socket
 */

#ifndef MESHER_HPP_
#define MESHER_HPP_

#include <boost/shared_ptr.hpp>

#include "common/Mesh.hpp"

template < typename T >
class Mesher {

public:
	typedef boost::shared_ptr< Mesher > Ptr;
	
public:
	Mesher() { }
	virtual ~Mesher() { }
	
	virtual Mesh::Ptr buildMesh(T data) =0;
	
};


#endif /* MESHER_HPP_ */
