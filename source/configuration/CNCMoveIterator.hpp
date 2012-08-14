/*
 * CNCMoveIterator.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef CNCMOVEITERATOR_HPP_
#define CNCMOVEITERATOR_HPP_

#include <iterator>

#include "common/Rototraslation.hpp"

class CNCMove {
	
public:
	CNCMove(const Rototraslation &stock, const Rototraslation &cutter) :
		STOCK(stock), CUTTER(cutter) { };
	virtual ~CNCMove() { }
	
	const Rototraslation STOCK, CUTTER;
};

class CNCMoveIterator: public std::iterator<std::input_iterator_tag, CNCMove> {
public:
	CNCMoveIterator();
	virtual ~CNCMoveIterator();
};

#endif /* CNCMOVEITERATOR_HPP_ */
