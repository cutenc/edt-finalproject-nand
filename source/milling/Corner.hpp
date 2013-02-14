/**
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

/**
 * @class Corner
 *
 * defines a corner of the voxel, among 8 available ones
 */
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

/**
 * @class CornerIterator
 *
 * iterates through the 8 corners of a cuboid
 */
class CornerIterator : public std::iterator<std::input_iterator_tag, Corner> {

private:
	static const Corner::CornerType END_FLAG = static_cast<Corner::CornerType>(-1);

	Corner::CornerType curr;
	
public:
	/**
	 * constructor
	 */
	CornerIterator() : curr(END_FLAG) { }

	/**
	 * constructor
	 * @param c
	 */
	CornerIterator(Corner::CornerType c) : curr(c) { }

	/**
	 * constructor
	 * @param oi
	 */
	CornerIterator(const CornerIterator &oi) : curr(oi.curr) { }
	
	/**
	 * destructor
	 */
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
	
	/**
	 * overrides ==
	 * @param oi
	 * @return
	 */
	bool operator==(const CornerIterator &oi) {
		return this->curr == oi.curr;
	}
	
	/**
	 * overrides !=
	 * @param oi
	 * @return
	 */
	bool operator!=(const CornerIterator &oi) {
		return this->curr != oi.curr;
	}
	
	/**
	 * overrides *
	 * @return
	 */
	Corner::CornerType operator*() {
		assert(!isEnded());
		
		return this->curr;
	}
	
	/**
	 *
	 * @return first corner (bottom-front-left)
	 */
	static CornerIterator begin() { return CornerIterator(Corner::BottomFrontLeft); }

	/**
	 *
	 * @return last corner (upper-rear-left)
	 */
	static CornerIterator end() { return CornerIterator(); }
	
private:
	bool isEnded() {
		return this->curr == CornerIterator::END_FLAG;
	}
};


#endif /* CORNER_HPP_ */
