/*
 * StockDescriptor.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef STOCKDESCRIPTOR_HPP_
#define STOCKDESCRIPTOR_HPP_

#include "Geometry.hpp"

class StockDescription {
public:
	StockDescription(const GeometryPtr desc);
	virtual ~StockDescription();
	
	const GeometryPtr getGeometry() const;
	
private:
	const GeometryPtr DESCRIPTION;
};

#endif /* STOCKDESCRIPTOR_HPP_ */
