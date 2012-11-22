/**
 * DisplayTextual.hpp
 *
 * Created on: 26/ott/2012
 * Author: socket
 *
 * displays textual infos of the operations, when the simulator is not launched in graphic mode
 */

#ifndef DISPLAYTEXTUAL_HPP_
#define DISPLAYTEXTUAL_HPP_

#include "threading/CyclicRunnable.hpp"

#include "InputDeviceStateType.hpp"
#include "MillingSignaler.hpp"

class DisplayTextual : public CyclicRunnable {
	
private:
	/** object needed by the scene updater*/
	const InputDeviceStateType::Ptr idst;
	/** infos of the milling ops */
	const MillingSignaler::Ptr signaler;
	/** is milling ended? */
	volatile bool millerEnd;
	
public:
	/**
	 * constructor
	 *
	 * @param idst : object needed by the scene updater
	 * @param signaler : infos of the milling ops
	 */
	DisplayTextual(const InputDeviceStateType::Ptr &idst, const MillingSignaler::Ptr signaler);

	/**
	 * destructor
	 */
	virtual ~DisplayTextual();
	
	/**
	 * to be executed on first iteration
	 */
	virtual void onBegin() throw();

	/**
	 * checks if milling has ended or has to continue
	 *
	 * @return !millerEnd : is milling going to continue?
	 */
	virtual bool hasNextCycle() throw();

	/**
	 * executes a cycle and prints the infos
	 *
	 * doCycle() waits for the miller, then runs through the queue
	 * and prints all the infos of the operations that the miller has performed,
	 * finally checks if milling is completed
	 */
	virtual void doCycle() throw();
};

#endif /* DISPLAYTEXTUAL_HPP_ */
