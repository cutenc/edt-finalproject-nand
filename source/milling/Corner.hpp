/*
 * Corner.hpp
 *
 *  Created on: 28/ago/2012
 *      Author: socket
 */

#ifndef CORNER_HPP_
#define CORNER_HPP_

#include <sys/types.h>
#include <iterator>
#include <stdexcept>
#include <cassert>

class Corner {
	
public:
	
	static const unsigned char N_CORNERS = 8;
	
	/**
	 *            Z     Y
	 *            |    / 
	 *        H---------G
	 *       /|   |  / /|
	 *      E---------F |
	 *      | |   |/  | |
	 *      | |   O---|-|---> X
	 *      | D-------|-C
	 *      |/        |/
	 *      A---------B
	 * 
	 * Origin is placed in the box centroid.
	 * Enum values are contiguous so it is legal to iterate through them or use as
	 * array indexes
	 */
	enum CornerType {
		
		/** A */
		BottomFrontLeft = 0,
		
		/** B */
		BottomFrontRight,
		
		/** C */
		BottomRearRight,
		
		/** D */
		BottomRearLeft,
		
		/** E */
		UpperFrontLeft,
		
		/** F */
		UpperFrontRight,
		
		/** G */
		UpperRearRight,
		
		/** H */
		UpperRearLeft
	};
	
};

class CornerIterator : public std::iterator<std::input_iterator_tag, Corner> {

private:
	static const Corner::CornerType END_FLAG = static_cast<Corner::CornerType>(-1);

	Corner::CornerType curr;
	
public:
	CornerIterator() : curr(END_FLAG) { }
	CornerIterator(Corner::CornerType c) : curr(c) { }
	CornerIterator(const CornerIterator &oi) : curr(oi.curr) { }
	
	virtual ~CornerIterator() { }
	
	CornerIterator& operator++() {
		assert(!isEnded());
		
		if (this->curr == Corner::UpperRearLeft) {
			this->curr = CornerIterator::END_FLAG;
		} else {
			int tmp = this->curr;
			this->curr = static_cast<Corner::CornerType>(++tmp);
		}
		
		return *this;
	}
	
	bool operator==(const CornerIterator &oi) {
		return this->curr == oi.curr;
	}
	
	bool operator!=(const CornerIterator &oi) {
		return this->curr != oi.curr;
	}
	
	Corner::CornerType operator*() {
		assert(!isEnded());
		
		return this->curr;
	}
	
	static CornerIterator begin() { return CornerIterator(Corner::BottomFrontLeft); }
	static CornerIterator end() { return CornerIterator(); }
	
private:
	bool isEnded() {
		return this->curr == CornerIterator::END_FLAG;
	}
};


#endif /* CORNER_HPP_ */
