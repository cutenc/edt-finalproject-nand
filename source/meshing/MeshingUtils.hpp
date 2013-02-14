/**
 * MeshingUtils.hpp
 *
 *  Created on: 22/ott/2012
 *      Author: socket
 */

#ifndef MESHINGUTILS_HPP_
#define MESHINGUTILS_HPP_

#include <cmath>

#include <boost/noncopyable.hpp>

#include <Eigen/Geometry>

#include <osg/Object>
#include <osg/Geometry>

#include "common/Utilities.hpp"
#include "milling/ShiftedBox.hpp"

/**
 * @class MeshingVoxel
 *
 * contains some utils to support meshing
 */
class MeshingUtils : boost::noncopyable {
public:
	
	/**
	 *
	 * @param halfExtents
	 * @param sbox
	 * @return True if the voxel is a boundary one
	 */
	static bool isBorderVoxel(const Eigen::Vector3d &halfExtents, const ShiftedBox &sbox);
	
	/**
	 * 
	 * @param halfExtents
	 * @param corner threated as a shifted box corner, that is a corner which
	 * coords belong to <tt>[-halfExtents, +halfExtents]</tt>
	 * @return True if the corner is a boundary one
	 */
	inline
	static bool isBorderCorner(const Eigen::Vector3d &halfExtents, const Eigen::Vector3d &corner) {
		for (int r = 0; r < 3; ++r) {
			if (CommonUtils::doubleEquals(
					halfExtents(r),
					fabs(corner(r)))
			) {
				return true;
			}
		}
		
		return false;
	}
	
	/**
	 *
	 * @param node
	 * @return data associated to the user
	 */
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
