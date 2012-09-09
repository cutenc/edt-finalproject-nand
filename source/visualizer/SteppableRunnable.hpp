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
	
public:
	typedef boost::shared_ptr< SteppableController > ControllerPtr;
	
private:
	const ControllerPtr CONTROLLER;
	
protected:
	virtual void doStep() =0;
	virtual bool isEnded() =0; 
	
public:
	SteppableRunnable(const ControllerPtr &controller);
	virtual ~SteppableRunnable();
	
};

#endif /* STEPPABLERUNNABLE_HPP_ */
