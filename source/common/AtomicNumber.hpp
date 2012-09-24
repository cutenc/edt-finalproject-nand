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
	mutable boost::shared_mutex mutex;
	
public:
	AtomicNumber() : number(0) { }
	virtual ~AtomicNumber() { }
	
	T get() const {
		SharedLock _(mutex);
		
		return this->number;
	}
	
	T set(const T n)  {
		UniqueLock _(mutex);
		
		T copy(number);
		number = n;
		
		return copy;
	}

	T addAndGet(const T n) {
		UniqueLock _(mutex);
		
		number += n;
		
		return number;
	}
	
	T getAndAdd(const T n) {
		UniqueLock _(mutex);
		
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
