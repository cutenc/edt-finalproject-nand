/*
 * SteppableController.cpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#include <limits>

#include "SteppableController.hpp"

SteppableController::SteppableController() :
	isStopped(false),
	isPaused(true),
	remainingStep( MAX_STEPS )
{ }

SteppableController::~SteppableController() {
}

bool SteppableController::canStep() {
	UniqueLock lock(mutex);
	
	while(shouldWait()) {
		awaitPlay.wait(lock);
	}
	
	remainingStep--;
	
	return !isStopped;
}

void SteppableController::stop() {
	UniqueLock _(mutex);
	isStopped = true;
	
	resume(_);
}

void SteppableController::pause() {
	UniqueLock _(mutex);
	isPaused = true;
}

void SteppableController::stepOnce() {
	step(1);
}

void SteppableController::step(u_long n) {
	UniqueLock _(mutex);
	
	remainingStep = n;
	
	resume(_);
}

void SteppableController::play() {
	step( MAX_STEPS );
}

void SteppableController::resume() {
	UniqueLock l(mutex);
	resume(l);
}

void SteppableController::resume(UniqueLock &lock) {
	isPaused = false;
	lock.unlock();
	
	awaitPlay.notify_all();
}

bool SteppableController::shouldWait() const {
	return (!isStopped) && (isPaused || (remainingStep == 0));
}

