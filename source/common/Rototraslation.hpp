/*
 * Rototraslation.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef ROTOTRASLATION_HPP_
#define ROTOTRASLATION_HPP_

#include "Point3D.hpp"
#include "EulerAngles.hpp"

class Rototraslation {
public:
	Rototraslation();
	Rototraslation(Point3D, EulerAngles);
	virtual ~Rototraslation();
	
	const Point3D TRASLATION;
	const EulerAngles ROTATION;
};

#endif /* ROTOTRASLATION_HPP_ */
