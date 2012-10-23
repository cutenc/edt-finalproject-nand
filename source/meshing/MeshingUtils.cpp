/*
 * MeshingUtils.cpp
 *
 *  Created on: 22/ott/2012
 *      Author: socket
 */

#include <cmath>

#include "common/Utilities.hpp"
#include "MeshingUtils.hpp"

MeshingUtils::MeshingUtils() { }

MeshingUtils::~MeshingUtils() { }

bool MeshingUtils::isBorderVoxel(const Eigen::Vector3d halfExtents,
		const ShiftedBox& sbox) {
	
	ShiftedBox::MinMaxMatrix minMax;
	
	ShiftedBox::buildMinMax(sbox, minMax);
	for(int r = 0; r < 3; ++r) {
		for (int c = 0; c < 2; c++) {
			if (CommonUtils::doubleEquals(
					halfExtents[r],
					fabs(minMax(r, c)))) {
				return true;
			}
		}
	}
	
	return false;
}

osg::Vec3d MeshingUtils::toOsg(const Eigen::Vector3d &vec) {
	return osg::Vec3d(vec[0], vec[1], vec[2]);
}

