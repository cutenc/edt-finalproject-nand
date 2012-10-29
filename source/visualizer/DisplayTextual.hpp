/*
 * DisplayTextual.hpp
 *
 *  Created on: 26/ott/2012
 *      Author: socket
 */

#ifndef DISPLAYTEXTUAL_HPP_
#define DISPLAYTEXTUAL_HPP_

#include "threading/CyclicRunnable.hpp"

#include "InputDeviceStateType.hpp"
#include "MillingSignaler.hpp"

class DisplayTextual : public CyclicRunnable {
	
private:
	const InputDeviceStateType::Ptr idst;
	const MillingSignaler::Ptr signaler;
	volatile bool millerEnd;
	
public:
	DisplayTextual(const InputDeviceStateType::Ptr &idst, const MillingSignaler::Ptr signaler);
	virtual ~DisplayTextual();
	
	virtual void onBegin() throw();
	virtual bool hasNextCycle() throw();
	virtual void doCycle() throw();
};

#endif /* DISPLAYTEXTUAL_HPP_ */
