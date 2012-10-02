/*
 * ThreadPool.hpp
 *
 *  Created on: 30/set/2012
 *      Author: socket
 */

#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#include <boost/utility.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>

class ThreadPool : boost::noncopyable {
	
public:
	typedef boost::function0< void > JobType;
	
private:
	boost::lock_guard< boost::mutex > LockType;
	
private:
	
	mutable boost::mutex mutex;
	boost::condition_variable workAvailable;
	boost::condition_variable workerEnded;
	std::deque< JobType > jobs;
	
public:
	ThreadPool(u_int nOfThreads);
	virtual ~ThreadPool();
	
	void queueJob(const JobType &job);
	
	void awaitFinish();
	
	void terminate();
	
private:
	void jobExecutor();
};

#endif /* THREADPOOL_HPP_ */
