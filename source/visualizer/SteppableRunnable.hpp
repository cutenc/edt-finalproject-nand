/*
 * SteppableRunnable.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef STEPPABLERUNNABLE_HPP_
#define STEPPABLERUNNABLE_HPP_

#include <boost/shared_ptr.hpp>

#include "CyclicRunnable.hpp"
#include "SteppableController.hpp"

class SteppableRunnable: public CyclicRunnable {
	
private:
	const SteppableController::Ptr CONTROLLER;
	
protected:
	virtual bool isEnded() throw() =0; 
	virtual void doCycle() throw() =0;
	
	virtual bool hasFinished() throw();
	
public:
	explicit SteppableRunnable(SteppableController::Ptr controller);
	virtual ~SteppableRunnable();
};

#endif /* STEPPABLERUNNABLE_HPP_ */
