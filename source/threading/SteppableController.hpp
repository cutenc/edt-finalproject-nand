/*
 * SteppableController.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef STEPPABLECONTROLLER_HPP_
#define STEPPABLECONTROLLER_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

class SteppableController {
	
public:
	typedef boost::shared_ptr< SteppableController > Ptr;
	
private:
	typedef boost::unique_lock< boost::mutex > UniqueLock;
	typedef boost::lock_guard< boost::mutex > LockGuard;
	
private:
	const static u_long MAX_STEPS;
	
	mutable boost::mutex mutex;
	boost::condition_variable awaitPlay;
	
	volatile bool isStopped;
	volatile bool isPaused;
	volatile u_long remainingStep;
	
public:
	SteppableController(bool startPaused = false);
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
	void resume();
	void stepOnce();
	void step(u_long n);
	
private:
	bool shouldWait() const;
	void resume(UniqueLock &lock);
	
};

#endif /* STEPPABLECONTROLLER_HPP_ */
