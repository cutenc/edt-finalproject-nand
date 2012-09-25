/*
 * SignaledInfo.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef SIGNALEDINFO_HPP_
#define SIGNALEDINFO_HPP_

#include "milling/MillingResult.hpp"

class SignaledInfo {
	
public:
	typedef std::deque< MillingResult > MillingData;
	typedef boost::shared_ptr< MillingData > MillingDataPtr;
	
public:
	SignaledInfo(MillingDataPtr results, const CNCMove &lastMove) :
		millingResults(results), lastMove(lastMove)
	{ }
	
	virtual ~SignaledInfo() { }
	
	MillingDataPtr millingResults;
	CNCMove lastMove;
};


#endif /* SIGNALEDINFO_HPP_ */
