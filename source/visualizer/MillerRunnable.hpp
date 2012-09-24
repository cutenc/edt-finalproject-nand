/*
 * MillerRunnable.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef MILLERRUNNABLE_HPP_
#define MILLERRUNNABLE_HPP_

#include <deque>

#include "milling/MillingAlgorithm.hpp"
#include "milling/MillingAlgorithmConf.hpp"
#include "SteppableRunnable.hpp"
#include "MillingSignaler.hpp"

class MillerRunnable: public SteppableRunnable {
	
private:
	MillingSignaler::Ptr signaler;
	MillingAlgorithm::Ptr algorithm;
	
public:
	MillerRunnable(SteppableController::Ptr controller,
			MillingSignaler::Ptr signaler,
			MillingAlgorithm::Ptr algorithm);
	
	virtual ~MillerRunnable();
	
protected:
	virtual bool hasNextStep() throw();
	virtual void doCycle() throw();
	virtual void onEnd() throw();
	
};

#endif /* MILLERRUNNABLE_HPP_ */
