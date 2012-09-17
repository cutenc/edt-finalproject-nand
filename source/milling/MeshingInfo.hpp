/*
 * MeshingInfo.hpp
 *
 *  Created on: 05/set/2012
 *      Author: socket
 */

#ifndef MESHINGINFO_HPP_
#define MESHINGINFO_HPP_

#include <utility>
#include <deque>

#include <boost/shared_ptr.hpp>

#include "configuration/CNCMoveIterator.hpp"
#include "ShiftedBox.hpp"
#include "VoxelInfo.hpp"

class MeshingInfo {
	
public:
	typedef std::pair< ShiftedBox, VoxelInfo > VoxelPair;
	typedef std::deque< VoxelPair > VoxelData;
	typedef boost::shared_ptr< VoxelData > VoxelDataPtr;
	
public:
	MeshingInfo();
	virtual ~MeshingInfo();
	
	/**
	 * For each pair, VoxelInfo contains the informations about corners
	 * insideness and ShiftedBox their location, according to stock basis
	 * 
	 * @return
	 */
	VoxelData getInfos() const;
	CNCMove getLastMove() const;
	
};

#endif /* MESHINGINFO_HPP_ */
