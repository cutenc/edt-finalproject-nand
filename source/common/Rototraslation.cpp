/*
 * Rototraslation.cpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#include "Rototraslation.hpp"

Rototraslation::Rototraslation() {
}

Rototraslation::Rototraslation(const Point3D &traslation, 
		const EulerAngles &rotation) : 
	TRASLATION(traslation), ROTATION(rotation) {
}

Rototraslation::~Rototraslation() {
}

