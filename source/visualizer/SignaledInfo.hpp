/**
 * @file SignaledInfo.hpp
 *
 * Created on: 07/set/2012
 * Author: socket
 *
 * contains the infos describing the state of the milling operations
 */

#ifndef SIGNALEDINFO_HPP_
#define SIGNALEDINFO_HPP_

#include "milling/MillingResult.hpp"

class SignaledInfo {
	
public:
	typedef std::deque< MillingResult > MillingData;
	typedef boost::shared_ptr< MillingData > MillingDataPtr;
	
	/**
	 * the three possible states the system can be in
	 * (I'm not sure the syntax is correct...)
	 *
	 * - is working, has infos
	 * - a timeout occurred
	 * - milling is ended, hooray
	 */
	enum State {
		HAS_DATA,  //!< HAS_DATA
		TIMEOUT,   //!< TIMEOUT
		MILLING_END//!< MILLING_END
	};
	
public:
	/**
	 * constructor
	 *
	 * @param state : one of the 3 states listed above
	 */
	SignaledInfo(State state) : state(state)
	{ }
	
	/**
	 * constructor
	 *
	 * @param results : list of the infos of all the milling operations performed since the last update
	 * @param lastMove : the last move of stock&cutter (has the rototraslations we're interested in)
	 */
	SignaledInfo(MillingDataPtr results, const CNCMove &lastMove) :
		state(HAS_DATA), millingResults(results), lastMove(lastMove)
	{ }
	
	virtual ~SignaledInfo() { }
	
	/** the state the system is actually in */
	const State state;

	/** list of the infos of all the milling operations performed since the last update */
	const MillingDataPtr millingResults;
	/** the last move of stock&cutter (has the rototraslations we're interested in) */
	const CNCMove lastMove;
};


#endif /* SIGNALEDINFO_HPP_ */
