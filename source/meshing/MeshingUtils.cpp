/*
 * MeshingUtils.cpp
 *
 *  Created on: 22/ott/2012
 *      Author: socket
 */

#include "MeshingUtils.hpp"

MeshingUtils::MeshingUtils() { }

MeshingUtils::~MeshingUtils() { }

bool MeshingUtils::isBorderVoxel(const Eigen::Vector3d &halfExtents,
		const ShiftedBox& sbox) {

	const ShiftedBox::MinMaxMatrix &minMax = sbox.getMatrix();

	for(int r = 0; r < 3; ++r) {
		for (int c = 0; c < 2; ++c) {
			if (CommonUtils::doubleEquals(
					(double)halfExtents[r],
					fabs((double)minMax(r, c)))) {
				return true;
			}
		}
	}

	return false;
}

