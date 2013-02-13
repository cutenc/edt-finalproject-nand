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
/**
 * @class AtomicNumber
 */
class AtomicNumber {
	
private:
	/* there's no need to use a shared lock because it is more expensive than
	 * a lock guard and here operations are very fast
	 */
	typedef boost::lock_guard< boost::mutex > LockGuard;
	
private:
	
	mutable boost::mutex mutex;
	volatile T number;
	
public:
	AtomicNumber() : number(0) { }
	AtomicNumber(T val) : number(val) { }
	virtual ~AtomicNumber() { }
	
	T get() const {
		LockGuard _(mutex);
		
		return this->number;
	}
	
	T set(const T n)  {
		LockGuard _(mutex);
		
		T copy(number);
		number = n;
		
		return copy;
	}

	T addAndGet(const T n) {
		LockGuard _(mutex);
		
		number += n;
		
		return number;
	}
	
	T getAndAdd(const T n) {
		LockGuard _(mutex);
		
		T copy(number);
		number += n;
		
		return copy;
	}
	
	T incAndGet() { return addAndGet(1); }
	T getAndInc() { return getAndAdd(1); }
	T decAndGet() { return addAndGet(-1); }
	T getAndDec()  { return getAndAdd(-1); }
	
	/* there's no ++ and -- operators because they would have to return a T
	 * reference instead of an AtmoicNumber< T > and this could be ambigous
	 */
};


#endif /* ATOMICNUMBER_HPP_ */
