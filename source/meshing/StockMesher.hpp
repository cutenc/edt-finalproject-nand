/*
 * StockMesher.hpp
 *
 *  Created on: 22/set/2012
 *      Author: socket
 */

#ifndef STOCKMESHER_HPP_
#define STOCKMESHER_HPP_

#include "Mesher.hpp"

#include "milling/Octree.hpp"
#include "milling/VoxelInfo.hpp"

class StockMesher: public Mesher< typename Octree<VoxelInfo>::DataView > {
	
public:
	typedef typename Octree< VoxelInfo >::DataView VoxelInfoDataView;
	typedef Mesher< VoxelInfoDataView >::Ptr Ptr;
	
public:
	StockMesher();
	virtual ~StockMesher();
	
	virtual Mesh::Ptr buildMesh(const VoxelInfoDataView &data);
};

#endif /* STOCKMESHER_HPP_ */
