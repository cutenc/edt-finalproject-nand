/*
 * Voxel.cpp
 *
 *  Created on: 09/set/2012
 *      Author: socket
 */

#include "Voxel.hpp"

Voxel::CornerTypeArray Voxel::buildDefaultOrder() {
	Voxel::CornerTypeArray tmp;

	int i = 0;
	CornerIterator cit = CornerIterator::begin();
	for(; cit != CornerIterator::end(); ++cit) {
		tmp[i++] = *cit;
	}
	
	return tmp;
}

const Voxel::CornerTypeArray Voxel::DEFAULT_ORDER = Voxel::buildDefaultOrder();
