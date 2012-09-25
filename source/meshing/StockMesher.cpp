/*
 * StockMesher.cpp
 *
 *  Created on: 22/set/2012
 *      Author: socket
 */

#include "StockMesher.hpp"

#include <sstream>

#include "common/meshes.hpp"

StockMesher::StockMesher() {
}

StockMesher::~StockMesher() {
}

Mesh::Ptr StockMesher::buildMesh(const VoxelInfoDataView &data) {
	std::stringstream ss;
	ss << "# of leaves: " << data.getStoredData()->size();
	
	return boost::make_shared< StockMesh >(ss.str());
}
