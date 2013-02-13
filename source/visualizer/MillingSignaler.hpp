/**
 * @file MillingSignaler.hpp
 *
 * Created on: 07/set/2012
 * Author: socket
 *
 * manages data communications between milling, meshing and viewer
 */

#ifndef MILLINGSIGNALER_HPP_
#define MILLINGSIGNALER_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "milling/MillingResult.hpp"
#include "configuration/CNCMoveIterator.hpp"
#include "SignaledInfo.hpp"

/**
 * @class MillingSignaler
 *
 * manages data communications between milling, meshing and viewer
 */
class MillingSignaler {
	
public:
	typedef boost::shared_ptr< MillingSignaler > Ptr;
	
private:
	typedef boost::unique_lock< boost::mutex > UniqueLock;
	typedef boost::lock_guard< boost::mutex > LockGuard;
	
	typedef SignaledInfo::MillingData MillingData;
	typedef SignaledInfo::MillingDataPtr MillingDataPtr;
	
	/**
	 * contains milling infos
	 */
	struct Predicate {
		/** last CNC move (has rototranslations of cutter&stock) */
		CNCMove lastMove;
		/** list of the result of the milling ops performed since last update */
		MillingDataPtr millingResults;
		/** is milling ended? */
		volatile bool millingEnd;

		/**
		 * constructor
		 */
		Predicate() :
			lastMove(),
			millingResults(boost::make_shared< MillingData >()),
			millingEnd(false)
		{ }

		/**
		 * destructor - empty
		 */
		virtual ~Predicate() { }

		/**
		 * checks of there is any significant update in the scene (ops performed or terminated)
		 * @return true if a milling operation has been performed or if milling has terminated
		 */
		bool operator()() const {
			return (!millingResults->empty()) || millingEnd;
		}

		/**
		 * packs together the infos of the last operations
		 *
		 * @return the SignaledInfo object containing the milling infos
		 */
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
	/** variables for concurrency */
	mutable boost::condition_variable millingReady;
	mutable boost::mutex mutex;
	
	/**
	 * Guarded-by #mutex
	 */
	Predicate predicate;
	
public:
	/**
	 * constructor - empty
	 */
	MillingSignaler();

	/**
	 * destructor - empty
	 */
	virtual ~MillingSignaler();
	
	/**
	 * waits for the milling infos of the max time available before updating the scene
	 *
	 * @return the infos of the milling ops performed since last update
	 */
	virtual SignaledInfo awaitMiller();
	
	/**
	 * waits for the milling infos of the given nap time before updating the scene
	 *
	 * @param duration : duration of the nap
	 * @return the infos of the milling ops performed since last update
	 */
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
