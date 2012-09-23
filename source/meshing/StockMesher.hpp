/*
 * StockMesher.hpp
 *
 *  Created on: 22/set/2012
 *      Author: socket
 */

#ifndef STOCKMESHER_HPP_
#define STOCKMESHER_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "Mesher.hpp"
#include "MeshingInfo.hpp"

class StockMesher: public Mesher< MeshingInfo::VoxelDataPtr > {
	
public:
	
	
public:
	StockMesher();
	virtual ~StockMesher();
};

#endif /* STOCKMESHER_HPP_ */
