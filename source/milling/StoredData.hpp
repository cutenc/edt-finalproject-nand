/*
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

class StoredData {
	
public:
	struct VoxelPair {
		VoxelPair(const ShiftedBox::ConstPtr &sBox, const VoxelInfo::Ptr vInfo) :
			sBox(sBox), vInfo(vInfo)
		{}
		virtual ~VoxelPair() { }
		
		const ShiftedBox::ConstPtr sBox;
		const VoxelInfo::Ptr vInfo;
	};
	
	typedef std::deque< VoxelPair > VoxelData;
	typedef boost::shared_ptr< VoxelData > VoxelDataPtr;
	
	typedef std::deque< VoxelInfo::Ptr > DeletedData;
	typedef boost::shared_ptr< DeletedData > DeletedDataPtr;
	
private:
	const VoxelDataPtr data;
	const DeletedDataPtr deleted;
	
public:
	StoredData(const VoxelDataPtr &data, const DeletedDataPtr &deleted);
	virtual ~StoredData();
	
	const VoxelDataPtr &getData() const;
	const DeletedDataPtr &getDeleted() const;
	
};
#endif /* STOREDDATA_HPP_ */
