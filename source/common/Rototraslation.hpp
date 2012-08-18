/*
 * Rototraslation.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef ROTOTRASLATION_HPP_
#define ROTOTRASLATION_HPP_

#include <iostream>

#include "Point3D.hpp"
#include "EulerAngles.hpp"

class Rototraslation {
public:
	Rototraslation();
	Rototraslation(const Point3D &, const EulerAngles &);
	virtual ~Rototraslation();
	
	friend std::ostream &operator<<(std::ostream &os, const Rototraslation &r) {
		os << "[T:" << r.TRASLATION << "R:" << r.ROTATION << "]";
		return os;
	}
	
	Point3D TRASLATION;
	EulerAngles ROTATION;
};

#endif /* ROTOTRASLATION_HPP_ */
