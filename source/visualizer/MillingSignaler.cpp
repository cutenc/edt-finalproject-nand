/*
 * MillingSignaler.cpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#include "MillingSignaler.hpp"

#include <boost/make_shared.hpp>

MillingSignaler::MillingSignaler() : 
	millingResults(boost::make_shared< SignaledInfo::MillingData >()),
	millingEnd(false)
{
}

MillingSignaler::~MillingSignaler() {
}

SignaledInfo MillingSignaler::awaitMiller() throw(MillerEndedException) {
	UniqueLock millingReadyLock(mutex);
	
	while(millingResults->empty() && (!millingEnd)) {
		millingReady.wait(millingReadyLock);
	}
	
	if (millingResults->empty()) {
		throw MillerEndedException();
	}
	
	SignaledInfo tmp(millingResults, lastMove);
	millingResults = boost::make_shared< SignaledInfo::MillingData >();
	
	return tmp;
}

void MillingSignaler::signalMesher(const MillingResult& result, const CNCMove &move) {
	{
		LockGuard millingLock(mutex);
	
		millingResults->push_back(result);
		lastMove = move;
	}
	
	millingReady.notify_all();
}

void MillingSignaler::signalMesher() {
	{
		LockGuard _(mutex);
		millingEnd = true;
	}
	
	millingReady.notify_all();
}
