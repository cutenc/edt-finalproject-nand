/*
 * CyclicRunnable.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef CYCLICRUNNABLE_HPP_
#define CYCLICRUNNABLE_HPP_

#include <boost/interprocess/detail/atomic.hpp>

#include "Runnable.hpp"

class CyclicRunnable: public Runnable {
	
	u_int cycleCount;
	
protected:
	virtual bool hasFinished() =0;
	
	/**
	 * Performs a processing cycle
	 */
	virtual void doCycle() =0;
	
public:
	CyclicRunnable();
	virtual ~CyclicRunnable();
	
	void run();
	
	u_int getCycleCount() const;
	
private:
	void atomicIncCylce();
	u_int atomicGetCycle() const;
};

#endif /* CYCLICRUNNABLE_HPP_ */
