/*
 * EulerAngles.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef EULERANGLES_HPP_
#define EULERANGLES_HPP_

#include <iostream>

class EulerAngles {
public:
	EulerAngles();
	EulerAngles(double, double, double);
	
	virtual ~EulerAngles();
	
	friend std::ostream &operator<<(std::ostream &os, const EulerAngles &ea) {
		os << "(" << ea.ALPHA << "," << ea.BETA << "," << ea.GAMMA << ")";
		return os;
	}
	
	double ALPHA, BETA, GAMMA;
};

#endif /* EULERANGLES_HPP_ */
