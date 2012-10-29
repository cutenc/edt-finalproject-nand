/*
 * Adjacencies.hpp
 *
 *  Created on: 17/ott/2012
 *      Author: socket
 */

#ifndef ADJACENCIES_HPP_
#define ADJACENCIES_HPP_

#include <sys/types.h>
#include <utility>

#include "Corner.hpp"

class Adjacencies {
	
public:
	
	enum AdjacencyType {
		LOCAL,
		EXTERN
	};
	
	enum Axis {
		X = 0,
		Y,
		Z
	};
	
	enum Verso {
		POSITIVE = 0,
		NEGATIVE
	};
	
	typedef std::pair<Axis, Verso> Direction;
	typedef std::pair< unsigned char, AdjacencyType > Adjacency;
	
	static Adjacency getAdjacent(unsigned char c, const Direction &dir);
	
};


#endif /* ADJACENCIES_HPP_ */
