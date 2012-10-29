/*
 * CyclicRunnable.cpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#include "CyclicRunnable.hpp"

CyclicRunnable::CyclicRunnable() {

}

CyclicRunnable::~CyclicRunnable() {
}

void CyclicRunnable::run() {
	
	onBegin();
	
	while (hasNextCycle()) {
		this->cycleCounter.incAndGet();
		
		this->doCycle();
	}
	
	onEnd();
}

unsigned long CyclicRunnable::getCycleCount() {
	return this->cycleCounter.get();
}


