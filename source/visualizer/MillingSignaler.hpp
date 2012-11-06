/*
 * MillingSignaler.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef MILLINGSIGNALER_HPP_
#define MILLINGSIGNALER_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "milling/MillingResult.hpp"
#include "configuration/CNCMoveIterator.hpp"
#include "SignaledInfo.hpp"

class MillingSignaler {
	
public:
	typedef boost::shared_ptr< MillingSignaler > Ptr;
	
private:
	typedef boost::unique_lock< boost::mutex > UniqueLock;
	typedef boost::lock_guard< boost::mutex > LockGuard;
	
	typedef SignaledInfo::MillingData MillingData;
	typedef SignaledInfo::MillingDataPtr MillingDataPtr;
	
	struct Predicate {
		CNCMove lastMove;
		MillingDataPtr millingResults;
		volatile bool millingEnd;
		
		Predicate() :
			lastMove(),
			millingResults(boost::make_shared< MillingData >()),
			millingEnd(false)
		{ }
		
		virtual ~Predicate() { }
		
		bool operator()() const {
			return (!millingResults->empty()) || millingEnd;
		}
		
		SignaledInfo buildInfo() {
			if (millingResults->empty()) {
				if (millingEnd) {
					return SignaledInfo(SignaledInfo::MILLING_END);
				} else {
					return SignaledInfo(SignaledInfo::TIMEOUT);
				}
			} // else, we have data to return
			
			SignaledInfo tmp(millingResults, lastMove);
			millingResults = boost::make_shared< SignaledInfo::MillingData >();
			
			return tmp;
		}
	};
	
private:
	mutable boost::condition_variable millingReady;
	mutable boost::mutex mutex;
	
	/*
	 * Guarded-by #mutex
	 */
	Predicate predicate;
	
public:
	MillingSignaler();
	virtual ~MillingSignaler();
	
	virtual SignaledInfo awaitMiller();
	
	template <typename Duration>
	SignaledInfo awaitMiller(const Duration &duration) {
		UniqueLock millingReadyLock(mutex);
		
		millingReady.timed_wait(millingReadyLock, duration, predicate);
		
		return predicate.buildInfo();
	}
	
	/**
	 * Used to tell awaiting meshers that miller has finished its work and
	 * there will be no more mills.
	 */
	virtual void signalMesher();
	
	/**
	 * Send a new mill result to the mesher
	 * @param results
	 */
	virtual void signalMesher(const MillingResult &results, const CNCMove &move);
	
};


#endif /* MILLINGSIGNALER_HPP_ */
