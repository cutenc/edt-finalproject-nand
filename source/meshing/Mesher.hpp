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
#include "common/StockMesh.hpp"
#include "milling/Octree.hpp"

template < typename T >
class Mesher {

	//Octree< VoxelInfo, DataTraits< VoxelInfo > >::DataView voxels;

public:
	typedef boost::shared_ptr< Mesher< T > > Ptr;
	typedef boost::shared_ptr< Mesher< T > > ConstPtr;
	
public:
	Mesher() { }
	virtual ~Mesher() { }
	
	virtual Mesh::Ptr buildMesh(const T &data) =0;
	
};


#endif /* MESHER_HPP_ */
