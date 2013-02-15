/**
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

/**
 * @class SteppableRunnable
 *
 * Class to advance the operations one step a time
 */
class SteppableRunnable: public CyclicRunnable {
	
private:
	const SteppableController::Ptr CONTROLLER;
	
protected:
	/**
	 *
	 * @return True if there are milling operations to do, False if milling has ended
	 */
	virtual bool hasNextStep() throw() =0;

	/**
	 * execute a step
	 */
	virtual void doCycle() throw() =0;
	
	/**
	 *
	 * @return True if milling can advance
	 */
	bool hasNextCycle() throw();
	
public:
	/**
	 * constructor
	 *
	 * @param controller
	 */
	explicit SteppableRunnable(SteppableController::Ptr controller);

	/**
	 * destructor
	 */
	virtual ~SteppableRunnable();
};

#endif /* STEPPABLERUNNABLE_HPP_ */
