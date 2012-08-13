/*
 * Rototraslation.cpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#include "Rototraslation.hpp"

Rototraslation::Rototraslation() {
}

Rototraslation::Rototraslation(Point3D traslation, EulerAngles rotation) : 
	TRASLATION(traslation), ROTATION(rotation) {
}

Rototraslation::~Rototraslation() {
}

