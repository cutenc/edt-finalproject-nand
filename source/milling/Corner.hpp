/*
 * Corner.hpp
 *
 *  Created on: 28/ago/2012
 *      Author: socket
 */

#ifndef CORNER_HPP_
#define CORNER_HPP_

#include <iterator>
#include <stdexcept>

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
enum Corner {
	
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

class CornerIterator : public std::iterator<std::input_iterator_tag, Corner> {

public:
	
	static const u_char N_CORNERS = 8;
	
private:
	static const Corner END_FLAG = static_cast<Corner>(-1);

	Corner curr;
	
public:
	CornerIterator() : curr(END_FLAG) { }
	CornerIterator(Corner c) : curr(c) { }
	CornerIterator(const CornerIterator &oi) : curr(oi.curr) { }
	
	virtual ~CornerIterator() { }
	
	CornerIterator& operator++() {
		if (isEnded()) {
			throw std::runtime_error("iterator ended: cannot ++");
		}
		
		if (this->curr == UpperRearLeft) {
			this->curr = CornerIterator::END_FLAG;
		} else {
			int tmp = this->curr;
			this->curr = static_cast<Corner>(++tmp);
		}
		
		return *this;
	}
	
	bool operator==(const CornerIterator &oi) {
		return this->curr == oi.curr;
	}
	
	bool operator!=(const CornerIterator &oi) {
		return this->curr != oi.curr;
	}
	
	Corner operator*() {
		if (isEnded()) {
			throw std::runtime_error("iterator ended: cannot dereference it");
		}
		
		return this->curr;
	}
	
	static CornerIterator begin() { return CornerIterator(BottomFrontLeft); }
	static CornerIterator end() { return CornerIterator(); }
	
private:
	bool isEnded() {
		return this->curr == CornerIterator::END_FLAG;
	}
};


#endif /* CORNER_HPP_ */
