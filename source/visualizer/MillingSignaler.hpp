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
#include "MillerEndedException.hpp"
#include "SignaledInfo.hpp"

class MillingSignaler {
	
public:
	typedef boost::shared_ptr< MillingSignaler > Ptr;
	
private:
	typedef boost::unique_lock< boost::mutex > UniqueLock;
	typedef boost::lock_guard< boost::mutex > LockGuard;
	
private:
	SignaledInfo::MillingDataPtr millingResults;
	CNCMove lastMove;
	volatile bool millingEnd;
	
	boost::condition_variable millingReady;
	boost::mutex mutex;
	
public:
	MillingSignaler();
	virtual ~MillingSignaler();
	
	virtual SignaledInfo awaitMiller() throw(MillerEndedException);
	
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
	
private:
	
	
};

#endif /* MILLINGSIGNALER_HPP_ */
