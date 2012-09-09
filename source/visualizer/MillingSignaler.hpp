/*
 * MillingSignaler.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef MILLINGSIGNALER_HPP_
#define MILLINGSIGNALER_HPP_

#include <boost/signals.hpp>
#include <boost/thread.hpp>

#include "milling/MeshingInfo.hpp"
#include "milling/MillingResult.hpp"

class MillingSignaler {
	
private:
	struct Slot {
		void operator()(const MillingResult &res) {
			// acquire lock on milling results
			// add res to milling result
			// release lock on milling results
			
			// acquire lock on sleep condition
			// awake any sleeping miller
			// release lock on sleep condition
		}
	};
	
	
	MillingDataPtr millingResults;
	const Slot SLOT;
	boost::signal<void (const MillingResult &)> SIGNAL;
	boost::condition_variable millingReady;
	boost::mutex millingReadyMutex;
	boost::mutex millinResultsMutex;
	
public:
	MillingSignaler();
	virtual ~MillingSignaler();
	
	SignaledInfo awaitMiller();
	void signalMesher(const MillingResult &results);
	
private:
	
	
};

#endif /* MILLINGSIGNALER_HPP_ */
