/*
 * MeshingUtils.hpp
 *
 *  Created on: 22/ott/2012
 *      Author: socket
 */

#ifndef MESHINGUTILS_HPP_
#define MESHINGUTILS_HPP_

#include <boost/noncopyable.hpp>

#include <Eigen/Geometry>

#include <osg/Object>
#include <osg/Geometry>

#include "milling/ShiftedBox.hpp"

class MeshingUtils : boost::noncopyable {
public:
	
	static bool isBorderVoxel(const Eigen::Vector3d &halfExtents, const ShiftedBox &sbox);
	
	template < typename DataT > inline
	static DataT * getUserData(osg::Object * node) {
		assert(dynamic_cast< DataT * >(node->getUserData()) != NULL);
		return static_cast< DataT * >(node->getUserData());
	}
	
private:
	MeshingUtils();
	virtual ~MeshingUtils();
};

#endif /* MESHINGUTILS_HPP_ */
