/*
 * CyclicRunnable.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef CYCLICRUNNABLE_HPP_
#define CYCLICRUNNABLE_HPP_

#include <boost/interprocess/detail/atomic.hpp>

#include "common/AtomicNumber.hpp"
#include "Runnable.hpp"

class CyclicRunnable: public Runnable {
	
private:
	AtomicNumber<u_long> cycleCounter;
	
protected:
	virtual bool hasFinished() throw() =0;
	
	/**
	 * Performs a processing cycle
	 */
	virtual void doCycle() throw() =0;
	
	/**
	 * This hook method will be called before any call to #hasFinished or
	 * #doCycle methods. A default empty implementation is already provided
	 * (it isn't abstract).
	 */
	virtual void onBegin() throw() { }
	
	/**
	 * This hook method will be called when the #hasFinished method returns
	 * true. A default empty implementation is already provided (it isn't
	 * abstract).
	 */
	virtual void onEnd() throw() { }
	
public:
	CyclicRunnable();
	virtual ~CyclicRunnable();
	
	void run();
	
	u_long getCycleCount();
	
};

#endif /* CYCLICRUNNABLE_HPP_ */
