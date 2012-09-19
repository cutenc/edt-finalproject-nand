/*
 * AtomicNumber.hpp
 *
 *  Created on: 19/set/2012
 *      Author: socket
 */

#ifndef ATOMICNUMBER_HPP_
#define ATOMICNUMBER_HPP_

#include <boost/thread.hpp>

template < typename T >
class AtomicNumber {
	
private:
	typedef boost::shared_lock< boost::shared_mutex > SharedLock;
	typedef boost::unique_lock< boost::shared_mutex > UniqueLock;
	
private:
	
	T number;
	boost::shared_mutex mutex;
	
public:
	AtomicNumber();
	virtual ~AtomicNumber();
	
	T get() const;
	T incAndGet();
	T getAndInc();
	T decAndGet();
	T getAndDec();
	T addAndGet(const T &n);
	T getAndAdd(const T &n);
};

#endif /* ATOMICNUMBER_HPP_ */
