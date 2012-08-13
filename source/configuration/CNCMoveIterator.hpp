/*
 * CNCMoveIterator.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef CNCMOVEITERATOR_HPP_
#define CNCMOVEITERATOR_HPP_

#include <iterator>

#include "CNCMove.hpp"

class CNCMoveIterator: public std::iterator<std::input_iterator_tag, CNCMove> {
public:
	CNCMoveIterator();
	virtual ~CNCMoveIterator();
};

#endif /* CNCMOVEITERATOR_HPP_ */
