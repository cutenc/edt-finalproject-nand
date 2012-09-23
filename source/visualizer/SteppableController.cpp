/*
 * SteppableController.cpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#include <limits>

#include "SteppableController.hpp"

SteppableController::SteppableController() :
	isStopped(false), isPaused(true), remainingStep( std::numeric_limits<u_long>::max() ){
}

SteppableController::~SteppableController() {
}

bool SteppableController::canStep() {
	UniqueLock lock(mutex);
	
	while(shouldWait()) {
		awaitPlay.wait(lock);
	}
	
	remainingStep--;
	
	return isStopped;
}

void SteppableController::stop() {
	UniqueLock _(mutex);
	isStopped = true;
	
	resume();
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
	
	resume();
}

void SteppableController::play() {
	step( std::numeric_limits<u_long>::max() );
}

void SteppableController::resume() {
	UniqueLock lock(mutex);
	
	isPaused = false;
	awaitPlay.notify_all();
}

bool SteppableController::shouldWait() const {
	return isPaused && (!isStopped) && (remainingStep <= 0);
}

