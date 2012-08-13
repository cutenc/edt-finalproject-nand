/*
 * EulerAngles.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef EULERANGLES_HPP_
#define EULERANGLES_HPP_

class EulerAngles {
public:
	EulerAngles();
	EulerAngles(double, double, double);
	EulerAngles(EulerAngles &);
	
	virtual ~EulerAngles();
	
	const double ALPHA, BETA, GAMMA;
};

#endif /* EULERANGLES_HPP_ */
