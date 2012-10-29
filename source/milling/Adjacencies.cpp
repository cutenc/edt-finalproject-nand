/*
 * Adjacencies.cpp
 *
 *  Created on: 17/ott/2012
 *      Author: socket
 */

#include "Adjacencies.hpp"

Adjacencies::Adjacency Adjacencies::getAdjacent(unsigned char corner, const Direction &dir) {
	
	static unsigned char directionLookup[][3] = {
	/* 0 */	{1, 3, 4},
	/* 1 */	{0, 2, 5},
	/* 2 */	{3, 1, 6},
	/* 3 */	{2, 0, 7},
	
	/* 4 */	{5, 7, 0},
	/* 5 */	{4, 6, 1},
	/* 6 */	{7, 5, 2},
	/* 7 */	{6, 4, 3}
	};
	
	static AdjacencyType adjTypeLookup[][3][2] = {
	/* 0 */	{
		/* X */	{LOCAL, EXTERN},
		/* Y */	{LOCAL, EXTERN},
		/* Z */	{LOCAL, EXTERN}
			},
	/* 1 */	{
		/* X */	{EXTERN, LOCAL},
		/* Y */	{LOCAL, EXTERN},
		/* Z */	{LOCAL, EXTERN}
			},
	/* 2 */	{
		/* X */	{EXTERN, LOCAL},
		/* Y */	{EXTERN, LOCAL},
		/* Z */	{LOCAL, EXTERN}
			},
	/* 3 */	{
		/* X */	{LOCAL, EXTERN},
		/* Y */	{EXTERN, LOCAL},
		/* Z */	{LOCAL, EXTERN}
			},
			// copy-paste of the upper 4 with Z reversed
	/* 4 */	{
		/* X */	{LOCAL, EXTERN},
		/* Y */	{LOCAL, EXTERN},
		/* Z */	{EXTERN, LOCAL}
			},
	/* 5 */	{
		/* X */	{EXTERN, LOCAL},
		/* Y */	{LOCAL, EXTERN},
		/* Z */	{EXTERN, LOCAL}
			},
	/* 6 */	{
		/* X */	{EXTERN, LOCAL},
		/* Y */	{EXTERN, LOCAL},
		/* Z */	{EXTERN, LOCAL}
			},
	/* 7 */	{
		/* X */	{LOCAL, EXTERN},
		/* Y */	{EXTERN, LOCAL},
		/* Z */	{EXTERN, LOCAL}
			}
	};
	
	int direction = static_cast< int >(dir.first);
	int verso = static_cast< int >(dir.second);
	
	return Adjacency(
			directionLookup[corner][direction],
			adjTypeLookup[corner][direction][verso]
	);
}
