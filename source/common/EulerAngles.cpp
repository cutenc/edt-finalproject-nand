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

EulerAngles::EulerAngles(double alpha, double beta, double gamma) :
	ALPHA(alpha), BETA(beta), GAMMA(gamma) {
}

EulerAngles::~EulerAngles() {
}

