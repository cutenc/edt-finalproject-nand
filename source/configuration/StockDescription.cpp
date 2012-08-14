/*
 * StockDescriptor.cpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#include "StockDescriptor.hpp"

StockDescription::StockDescription(const GeometryPtr desc) :
		DESCRIPTION(desc) {
}

StockDescription::~StockDescription() {
}

const GeometryPtr StockDescription::getGeometry() const {
	return DESCRIPTION;
}
