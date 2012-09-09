/*
 * MeshingInfo.hpp
 *
 *  Created on: 05/set/2012
 *      Author: socket
 */

#ifndef MESHINGINFO_HPP_
#define MESHINGINFO_HPP_

#include <utility>

#include "configuration/CNCMoveIterator.hpp"

class MeshingInfo {
	
public:
	typedef std::pair< ShiftedBox, VoxelInfo > VoxelData;
	
public:
	MeshingInfo();
	virtual ~MeshingInfo();
	
	/**
	 * For each pair, VoxelInfo contains the informations about corners
	 * insideness and ShiftedBox their location, according to stock basis
	 * 
	 * @return
	 */
	std::deque< VoxelData > getInfos() const;
	CNCMove getLastMove() const;
	
};

#endif /* MESHINGINFO_HPP_ */
