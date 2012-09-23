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
	
	while (!hasFinished()) {
		this->cycleCounter.incAndGet();
		
		this->doCycle();
	}
	
	onEnd();
}

u_long CyclicRunnable::getCycleCount() {
	return this->cycleCounter.get();
}


