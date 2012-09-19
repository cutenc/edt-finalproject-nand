/*
 * AtomicNumber.cpp
 *
 *  Created on: 19/set/2012
 *      Author: socket
 */

#include "AtomicNumber.hpp"

template<typename T>
inline AtomicNumber<T>::AtomicNumber() : number(0) {
}

template<typename T>
inline AtomicNumber<T>::~AtomicNumber() {
}

template<typename T>
inline T AtomicNumber<T>::get() const {
	SharedLock _(mutex);
	
	return this->number;
}

template<typename T>
inline T AtomicNumber<T>::incAndGet() {
	return addAndGet(1);
}

template<typename T>
inline T AtomicNumber<T>::getAndInc() {
	return getAndAdd(1);
}

template<typename T>
inline T AtomicNumber<T>::decAndGet() {
	return addAndGet(-1);
}

template<typename T>
inline T AtomicNumber<T>::getAndDec() {
	return getAndAdd(-1);
}

template<typename T>
inline T AtomicNumber<T>::addAndGet(const T& n) {
	UniqueLock _(mutex);
	
	number += n;
	
	return number;
}

template<typename T>
inline T AtomicNumber<T>::getAndAdd(const T& n) {
	UniqueLock _(mutex);
	
	T copy(number);
	number += n;
	
	return copy;
}



