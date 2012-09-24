/*
 * SteppableRunnable.cpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#include "SteppableRunnable.hpp"

SteppableRunnable::SteppableRunnable(SteppableController::Ptr controller) :
		CONTROLLER(controller)
{
}

SteppableRunnable::~SteppableRunnable() {
}

bool SteppableRunnable::hasNextCycle() throw() {
	return (CONTROLLER->canStep()) && this->hasNextStep();
}

const u_long SteppableController::MAX_STEPS = std::numeric_limits< u_long >::max();
