/**
 * StubMesher.hpp
 *
 *  Created on: 27/ott/2012
 *      Author: socket
 */

#ifndef STUBMESHER_HPP_
#define STUBMESHER_HPP_

#include "Mesher.hpp"

template < typename T >
/**
 * @class StubMesher
 *
 * Stub for testing purposes
 */
class StubMesher: public Mesher< T > {
public:
	StubMesher() { }
	virtual ~StubMesher() { }
	
	virtual Mesh::Ptr buildMesh(const T &) {
		throw std::runtime_error("THIS IS ONLY A STUPID STUB MESHER");
	}
};

#endif /* STUBMESHER_HPP_ */
