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
	const double MC_THRESHOLD = 0.0;
	const Eigen::Vector3d HALF_EXTENTS;
	const double RADIUS;
	const osg::ref_ptr< osg::Group > MESH;
	CubeCache cubeCache;
	
	double* StockMesher::getDistance(const Eigen::Vector3d point, SimpleBox::Ptr sbp);

public:
	StockMesher(const StockDescription &stock);
	virtual ~StockMesher();
	
	virtual Mesh::Ptr buildMesh(const StoredData &data);
};

#endif /* STOCKMESHER_HPP_ */
