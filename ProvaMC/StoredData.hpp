/*
 * StoredData.hpp
 *
 *  Created on: 22/ott/2012
 *      Author: alberto
 */

#ifndef STOREDDATA_HPP_
#define STOREDDATA_HPP_

#include <iostream>
#include <bits/stl_pair.h>
#include <utility>

#include "ShiftedBox.hpp"
#include "VoxelInfo.hpp"

class StoredData {

public:
	typedef std::pair< ShiftedBox::Ptr, boost::shared_ptr<VoxelInfo> > VoxelPair;
	typedef std::deque< VoxelPair > VoxelData;
	typedef boost::shared_ptr< VoxelData > VoxelDataPtr;

public:
	VoxelDataPtr data;

	StoredData() :
		data(boost::make_shared< VoxelData >())
	{ }

	StoredData(const VoxelDataPtr &data) :
		data(data)
	{ }

	virtual ~StoredData() { }

	VoxelDataPtr getData() const {
		return data;
	}

};

#endif /* STOREDDATA_HPP_ */
