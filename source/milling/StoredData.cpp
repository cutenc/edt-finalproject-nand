/*
 * StoredData.cpp
 *
 *  Created on: 22/ott/2012
 *      Author: socket
 */

#include "StoredData.hpp"


StoredData::StoredData(const VoxelDataPtr& data,
		const DeletedDataPtr& deleted) :
		data(data), deleted(deleted)
{
}

StoredData::~StoredData() {
}

const StoredData::VoxelDataPtr &StoredData::getData() const {
	return this->data;
}

const StoredData::DeletedDataPtr &StoredData::getDeleted() const {
	return this->deleted;
}
