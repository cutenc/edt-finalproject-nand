/*
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

class MeshingUtils : boost::noncopyable {
public:
	
	static bool isBorderVoxel(const Eigen::Vector3d &halfExtents, const ShiftedBox &sbox);
	
	/**
	 * 
	 * @param halfExtents
	 * @param corner threated as a shifted box corner, that is a corner which
	 * coords belong to <tt>[-halfExtents, +halfExtents]</tt>
	 * @return
	 */
	inline
	static bool isBorderCorner(const Eigen::Vector3d &halfExtents, const Eigen::Vector3d &corner) {
		for (int r = 0; r < 3; ++r) {
			if (CommonUtils::doubleEquals(
					(double)halfExtents(r),
					fabs((double)corner(r)))
			) {
				return true;
			}
		}
		
		return false;
	}
	
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
