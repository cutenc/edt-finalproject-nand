/*
 * SignaledInfo.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef SIGNALEDINFO_HPP_
#define SIGNALEDINFO_HPP_

class SignaledInfo {
	
public:
	typedef std::deque< MillingResult > MillingData;
	typedef boost::shared_ptr< MillingData > MillingDataPtr;
	
	SignaledInfo() { }
	virtual ~SignaledInfo() { }
	
	MillingDataPtr millingResults;
	MeshingInfo meshing;
};


#endif /* SIGNALEDINFO_HPP_ */
