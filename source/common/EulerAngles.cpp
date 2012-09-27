/*
 * EulerAngles.cpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#include "EulerAngles.hpp"

EulerAngles::EulerAngles() :
	ALPHA(0), BETA(0), GAMMA(0) {
}

EulerAngles::EulerAngles(float alpha, float beta, float gamma) :
	ALPHA(alpha), BETA(beta), GAMMA(gamma) {
}

EulerAngles::~EulerAngles() {
}

