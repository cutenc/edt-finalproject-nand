/*
 * SteppableController.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef STEPPABLECONTROLLER_HPP_
#define STEPPABLECONTROLLER_HPP_

class SteppableController {
	
public:
	SteppableController();
	virtual ~SteppableController();
	
	/**
	 * This method should be used only by processing thread (the controlled
	 * one). It stops the thread until it can continue execution returning
	 * \c true or \c false wether it should perform another step or exit
	 * due to a stop command
	 * @return \c true if processing can continue, \c false if it has to stop
	 */
	bool canStep();
	
	void stop();
	void play();
	void pause();
	void stepOnce();
	void step(u_int n);
};

#endif /* STEPPABLECONTROLLER_HPP_ */
