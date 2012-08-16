/*
 * CutterDescription.cpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#include "CutterDescription.hpp"

CutterDescription::CutterDescription(const GeometryPtr &desc, const Color& color) :
		DESCRIPTION(desc), COLOR(color) {
}

CutterDescription::~CutterDescription() {
}

const GeometryPtr CutterDescription::getGeometry() const {
	return DESCRIPTION;
}

Color CutterDescription::getColor() const {
	return COLOR;
}
