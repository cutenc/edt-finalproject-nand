/*
 * MesherStub.hpp
 *
 *  Created on: 24/set/2012
 *      Author: socket
 */

#ifndef MESHERSTUB_HPP_
#define MESHERSTUB_HPP_

#include <iostream>

#include "MillingSignaler.hpp"

class MesherStub : public CyclicRunnable {
	
private:
	const MillingSignaler::Ptr signaler;
	bool finished;
	
public:
	MesherStub(MillingSignaler::Ptr signaler) : signaler(signaler), finished(false)
	{ }
	
	virtual ~MesherStub() { }
	
protected:
	virtual bool hasFinished() throw() {
		return finished;
	}
	
	virtual void onBegin() throw() {
		std::cout << "Mesher up and running" << std::endl;
	}
	
	virtual void onEnd() throw() {
		std::cout << "Mesher saying good bye" << std::endl;
	}
	
	virtual void doCycle() throw() {
		try {
			SignaledInfo infos = signaler->awaitMiller();
			
			std::cout << infos.lastMove << std::endl;
			SignaledInfo::MillingData::const_iterator it;
			for (it = infos.millingResults->begin(); it != infos.millingResults->end(); ++it) {
				std::cout << *it << std::endl;
			}
			
		} catch (const MillerEndedException &e) {
			finished = true;
		}
	}
	
};

#endif /* MESHERSTUB_HPP_ */
