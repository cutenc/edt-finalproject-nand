/*
 * StockMesher.hpp
 *
 *  Created on: 22/set/2012
 *      Author: socket
 */

#ifndef STOCKMESHER_HPP_
#define STOCKMESHER_HPP_

#include "Mesher.hpp"

#include <Eigen/Geometry>

#include <osg/Group> 

#include "configuration/StockDescription.hpp"
#include "milling/StoredData.hpp"
#include "CubeCache.hpp"

class StockMesher: public Mesher< StoredData > {
	
public:
	typedef Mesher< StoredData >::Ptr Ptr;

private:
	const Eigen::Vector3d HALF_EXTENTS;
	const osg::ref_ptr< osg::Group > MESH;
	CubeCache cubeCache;
	
public:
	StockMesher(const StockDescription &stock);
	virtual ~StockMesher();
	
	virtual Mesh::Ptr buildMesh(const StoredData &data);
};

#endif /* STOCKMESHER_HPP_ */
