/*
 * DisplayTextual.cpp
 *
 *  Created on: 26/ott/2012
 *      Author: socket
 */

#include "DisplayTextual.hpp"

#include <cassert>
#include <iostream>

#include "SignaledInfo.hpp"

DisplayTextual::DisplayTextual(const InputDeviceStateType::Ptr& idst,
		const MillingSignaler::Ptr signaler) :
		idst(idst), signaler(signaler), millerEnd(false)
{ }

DisplayTextual::~DisplayTextual() {
}

bool DisplayTextual::hasNextCycle() throw() {
	return !this->millerEnd;
}

void DisplayTextual::onBegin() throw() {
	std::cout << MillingResult::getPrintHeader() << std::endl;
}

void DisplayTextual::doCycle() throw() {
	
	SignaledInfo sInfo = signaler->awaitMiller();
	
	switch (sInfo.state) {
	case SignaledInfo::HAS_DATA:
		if (idst->shouldUpdateScene()) {
			SignaledInfo::MillingData::const_iterator dataIt = sInfo.millingResults->begin();
			for (; dataIt != sInfo.millingResults->end(); ++dataIt) {
				std::cout << *dataIt << std::endl;
			}
		}
		break;
		
	case SignaledInfo::MILLING_END:
		idst->signalMillingEnd();
		millerEnd = true;
		break;
		
	default:
		/* This case should never happen but there may be spurious wakeups
		 * not trapped by boost itself
		 */
		break;
	}
}


