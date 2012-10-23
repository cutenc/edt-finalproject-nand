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

#include <osg/Geometry>

#include "milling/ShiftedBox.hpp"

class MeshingUtils : boost::noncopyable {
public:
	
	static bool isBorderVoxel(const Eigen::Vector3d halfExtents, const ShiftedBox &sbox);
	
	static osg::Vec3d toOsg(const Eigen::Vector3d &vec);
	
private:
	MeshingUtils();
	virtual ~MeshingUtils();
};

#endif /* MESHINGUTILS_HPP_ */
