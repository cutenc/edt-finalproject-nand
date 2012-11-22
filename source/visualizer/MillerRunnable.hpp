/**
 * @file MillerRunnable.hpp
 *
 * Created on: 07/set/2012
 * Author: socket
 *
 * interface for advancing the milling operations step-by-step
 */

#ifndef MILLERRUNNABLE_HPP_
#define MILLERRUNNABLE_HPP_

#include <deque>

#include "milling/MillingAlgorithm.hpp"
#include "milling/MillingAlgorithmConf.hpp"
#include "threading/SteppableRunnable.hpp"
#include "MillingSignaler.hpp"

class MillerRunnable: public SteppableRunnable {
	
private:
	/** manages data communications between milling, meshing and viewer */
	MillingSignaler::Ptr signaler;
	/** the miller */
	MillingAlgorithm::Ptr algorithm;
	
public:
	/**
	 * constructor
	 *
	 * @param controller : manages progression of operations
	 * @param signaler : manages data communications between milling, meshing and viewer
	 * @param algorithm : the miller
	 */
	MillerRunnable(SteppableController::Ptr controller,
			MillingSignaler::Ptr signaler,
			MillingAlgorithm::Ptr algorithm);
	
	/**
	 * destructor - empty
	 */
	virtual ~MillerRunnable();
	
protected:
	/**
	 * checks if miller has work to do
	 * @return true if miller has work to do, false if it has ended
	 */
	virtual bool hasNextStep() throw();

	/**
	 * executes a step of milling
	 */
	virtual void doCycle() throw();

	/**
	 * executed when miller ends its job
	 */
	virtual void onEnd() throw();
	
};

#endif /* MILLERRUNNABLE_HPP_ */
