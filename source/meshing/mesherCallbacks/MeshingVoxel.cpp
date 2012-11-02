/*
 * MeshingVoxel.cpp
 *
 *  Created on: 22/ott/2012
 *      Author: alberto
 */

#include "MeshingVoxel.hpp"

const Corner::CornerType MeshingVoxel::CORNER_CONVERSION[8] = {
		Corner::BottomRearLeft,		// 0
		Corner::BottomRearRight,	// 1
		Corner::BottomFrontRight,	// 2
		Corner::BottomFrontLeft,	// 3
		Corner::UpperRearLeft,		// 4
		Corner::UpperRearRight,		// 5
		Corner::UpperFrontRight,	// 6
		Corner::UpperFrontLeft		// 7
};

