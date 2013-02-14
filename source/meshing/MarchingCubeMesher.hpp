/**
 * MarchingCubeMesher.hpp
 *
 *  Created on: 26/ott/2012
 *      Author: socket
 */

#ifndef MARCHINGCUBEMESHER_HPP_
#define MARCHINGCUBEMESHER_HPP_

#include "CommonMesher.hpp"

#include "leaf_node_callbacks.hpp"

/**
 * @class MarchingCubeMesher
 *
 */
class MarchingCubeMesher : public CommonMesher {
	
private:
	static const unsigned int DEFAULT_LEAF_SIZE = 300;
	
public:
	MarchingCubeMesher(const StockDescription& stock) :
		CommonMesher(stock,
				new MarchingCubeMesherCallback(stock),
				DEFAULT_LEAF_SIZE
		)
	{ }
	
	virtual ~MarchingCubeMesher() { }
};

#endif /* MARCHINGCUBEMESHER_HPP_ */
