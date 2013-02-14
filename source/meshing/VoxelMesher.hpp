/**
 * VoxelMesher.hpp
 *
 *  Created on: 26/ott/2012
 *      Author: socket
 */

#ifndef VOXELMESHER_HPP_
#define VOXELMESHER_HPP_

#include "CommonMesher.hpp"

#include "leaf_node_callbacks.hpp"

/**
 * @class VoxelMesher
 *
 * creates a new box
 */
class VoxelMesher : public CommonMesher {
	
private:
	static const unsigned int DEFAULT_LEAF_SIZE = 400;
	
public:
	/**
	 * constructor
	 *
	 * @param stock
	 */
	VoxelMesher(const StockDescription& stock) :
		CommonMesher(stock,
				new BoxMesherCallback(stock),
				DEFAULT_LEAF_SIZE
		)
	{ }
	
	virtual ~VoxelMesher() { }
	
};


#endif /* VOXELMESHER_HPP_ */
