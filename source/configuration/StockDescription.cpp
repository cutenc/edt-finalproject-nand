/*
 * StockDescriptor.cpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#include "StockDescription.hpp"

StockDescription::StockDescription(const RectCuboidPtr& desc) :
		DESCRIPTION(desc) {
}

StockDescription::~StockDescription() {
}

const RectCuboidPtr StockDescription::getGeometry() const {
	return this->DESCRIPTION;
}
