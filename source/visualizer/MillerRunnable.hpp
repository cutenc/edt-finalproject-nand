/*
 * MillerRunnable.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef MILLERRUNNABLE_HPP_
#define MILLERRUNNABLE_HPP_

#include <deque>

#include "SteppableRunnable.hpp"
#include "milling/MeshingInfo.hpp"

class MillerRunnable: public SteppableRunnable {
	
public:
	MillerRunnable();
	virtual ~MillerRunnable();
	
	MeshingDataPtr getMeshingData();
};

#endif /* MILLERRUNNABLE_HPP_ */
