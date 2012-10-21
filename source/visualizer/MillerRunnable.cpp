/*
 * MillerRunnable.cpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#include "MillerRunnable.hpp"

#include <iostream>

#include "milling/MillingResult.hpp"

MillerRunnable::MillerRunnable(SteppableController::Ptr controller,
		MillingSignaler::Ptr signaler, MillingAlgorithm::Ptr alg) :
		SteppableRunnable(controller), signaler(signaler), algorithm(alg)
{ }

MillerRunnable::~MillerRunnable() {
}

bool MillerRunnable::hasNextStep() throw() {
	return algorithm->hasNextStep();
}

void MillerRunnable::doCycle() throw() {
	MillingAlgorithm::StepInfo res = algorithm->step();
	signaler->signalMesher(res.first, res.second);
}

void MillerRunnable::onEnd() throw() {
	signaler->signalMesher();
}

