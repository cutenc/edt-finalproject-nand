/*
 * VoxelMesher.hpp
 *
 *  Created on: 26/ott/2012
 *      Author: socket
 */

#ifndef COMMONMESHER_HPP_
#define COMMONMESHER_HPP_

#include "Mesher.hpp"

#include <Eigen/Geometry>

#include <osg/Group> 

#include "configuration/StockDescription.hpp"
#include "milling/StoredData.hpp"
#include "MeshOctree.hpp"

class CommonMesher: public Mesher< StoredData > {
	
public:
	typedef Mesher< StoredData >::Ptr Ptr;

private:
	const Eigen::Vector3d HALF_EXTENTS;
	MeshOctree meshOctree;
	
public:
	CommonMesher(const StockDescription &stock, LeafNodeCallback *lnc,
			unsigned int maxLeafSize, unsigned int maxDepth = 32);
	virtual ~CommonMesher();
	
	virtual Mesh::Ptr buildMesh(const StoredData &data);
};


#endif /* COMMONMESHER_HPP_ */
