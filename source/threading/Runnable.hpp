/**
 * Runnable.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef RUNNABLE_HPP_
#define RUNNABLE_HPP_

/**
 * @class Runnable
 *
 * basic runner to advance operations of any kind
 */
class Runnable {
	
public:
	/**
	 * destructor
	 */
	virtual ~Runnable() { }
	
	/**
	 * run operations
	 *
	 * (to be overridden)
	 */
	virtual void run() =0;
	
	/**
	 * MUST NOT BE OVERRIDDEN/OVERLOADED
	 */
	void operator()() {
		run();
	}
};


#endif /* RUNNABLE_HPP_ */
