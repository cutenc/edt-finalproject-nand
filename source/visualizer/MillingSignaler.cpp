/*
 * MillingSignaler.cpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#include "MillingSignaler.hpp"

#include <limits>

#include <boost/make_shared.hpp>
#include <boost/date_time.hpp>

MillingSignaler::MillingSignaler() {
}

MillingSignaler::~MillingSignaler() {
}

SignaledInfo MillingSignaler::awaitMiller() {
	const static boost::posix_time::seconds MAX_TIME(
			std::numeric_limits< long >::max()
	);
	
	return awaitMiller(MAX_TIME);
}

void MillingSignaler::signalMesher(const MillingResult& result, const CNCMove &move) {
	{
		LockGuard millingLock(mutex);
	
		predicate.millingResults->push_back(result);
		predicate.lastMove = move;
	}
	
	millingReady.notify_all();
}

void MillingSignaler::signalMesher() {
	{
		LockGuard _(mutex);
		predicate.millingEnd = true;
	}
	
	millingReady.notify_all();
}

