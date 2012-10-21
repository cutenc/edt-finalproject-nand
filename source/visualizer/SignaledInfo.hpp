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
	
	enum State {
		HAS_DATA,
		TIMEOUT,
		MILLING_END
	};
	
public:
	SignaledInfo(State state) : state(state)
	{ }
	
	SignaledInfo(MillingDataPtr results, const CNCMove &lastMove) :
		state(HAS_DATA), millingResults(results), lastMove(lastMove)
	{ }
	
	virtual ~SignaledInfo() { }
	
	const State state;
	const MillingDataPtr millingResults;
	const CNCMove lastMove;
};


#endif /* SIGNALEDINFO_HPP_ */
