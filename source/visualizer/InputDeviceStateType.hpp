/*
 * InputDeviceStateType.hpp
 *
 *  Created on: 10/ott/2012
 *      Author: alberto
 */

#ifndef INPUTDEVICESTATETYPE_HPP_
#define INPUTDEVICESTATETYPE_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

class InputDeviceStateType {
	
public:
	typedef boost::shared_ptr< InputDeviceStateType > Ptr;
	typedef boost::shared_ptr< const InputDeviceStateType > ConstPtr;
	
private:
	bool updateScene;
	
public:
	InputDeviceStateType();
	virtual ~InputDeviceStateType();
	
	bool shouldUpdateScene();
	
	void signalMillingEnd();
};

#endif /* INPUTDEVICESTATETYPE_HPP_ */
