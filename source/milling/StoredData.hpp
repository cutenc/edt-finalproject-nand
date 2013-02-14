/**
 * StoredData.hpp
 *
 *  Created on: 22/ott/2012
 *      Author: socket
 */

#ifndef STOREDDATA_HPP_
#define STOREDDATA_HPP_

#include <utility>
#include <deque>

#include <boost/shared_ptr.hpp>

#include "VoxelInfo.hpp"
#include "ShiftedBox.hpp"
#include "graphics_info.hpp"

/**
 * @class StoredData
 *
 * the data contained in a voxel
 */
class StoredData {
	
public:
	typedef GraphicData VoxelPair;
	typedef std::deque< VoxelPair > VoxelData;
	typedef boost::shared_ptr< VoxelData > VoxelDataPtr;
	
	typedef std::deque< VoxelInfo::Ptr > DeletedData;
	typedef boost::shared_ptr< DeletedData > DeletedDataPtr;
	
private:
	const VoxelDataPtr data;
	const DeletedDataPtr deleted;
	
public:
	/**
	 * constructor
	 * @param data
	 * @param deleted
	 */
	StoredData(const VoxelDataPtr &data, const DeletedDataPtr &deleted);

	/**
	 * destructor
	 */
	virtual ~StoredData();
	
	/**
	 *
	 * @return the data
	 */
	const VoxelDataPtr &getData() const;

	/**
	 *
	 * @return deleted data
	 */
	const DeletedDataPtr &getDeleted() const;
	
};
#endif /* STOREDDATA_HPP_ */
