/*
 * Runnable.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef RUNNABLE_HPP_
#define RUNNABLE_HPP_

class Runnable {
	
public:
	virtual ~Runnable() { }
	
	virtual void run() =0;
	
	/**
	 * MUST NOT BE OVERRIDED/OVERLOADED
	 */
	void operator()() {
		run();
	}
};


#endif /* RUNNABLE_HPP_ */
