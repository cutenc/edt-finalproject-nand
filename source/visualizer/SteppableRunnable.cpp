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

bool SteppableRunnable::hasFinished() throw() {
	// TODO il controller viene cortocircuitato qui!!
	return this->isEnded() && (!CONTROLLER->canStep());
}

