/*
 * MesherStub.hpp
 *
 *  Created on: 24/set/2012
 *      Author: socket
 */

#ifndef MESHERSTUB_HPP_
#define MESHERSTUB_HPP_

#include <iostream>

#include "threading/CyclicRunnable.hpp"
#include "MillingSignaler.hpp"

class MesherStub : public CyclicRunnable {
	
private:
	const MillingSignaler::Ptr signaler;
	Stock::Ptr stock;
	bool finished;
	
public:
	MesherStub(MillingSignaler::Ptr signaler, Stock::Ptr stock) :
		signaler(signaler), stock(stock), finished(false)
	{ }
	
	virtual ~MesherStub() { }
	
protected:
	virtual bool hasNextCycle() throw() {
		return !finished;
	}
	
	virtual void onBegin() throw() {
		std::cout << "Mesher up and running" << std::endl;
		std::cout << MillingResult::getPrintHeader() << std::endl;
	}
	
	virtual void onEnd() throw() {
		std::cout << "Mesher saying good bye" << std::endl;
	}
	
	virtual void doCycle() throw() {
		try {
			SignaledInfo infos = signaler->awaitMiller();
			
//			Mesh::Ptr mesh = stock->getMeshing();
//			std::cout << "mesh: " << *mesh << std::endl;
			
//			std::cout << infos.lastMove << std::endl;
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
