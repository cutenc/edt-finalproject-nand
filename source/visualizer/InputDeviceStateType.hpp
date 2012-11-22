/**
 * @file InputDeviceStateType.hpp
 *
 * Created on: 10/ott/2012
 * Author: alberto
 *
 * class needed by the scene updater
 * recognizes when the scene has to be updated
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
	/** should the scene be updated? */
	bool updateScene;
	
public:
	/**
	 * constructor - empty
	 * by default the scene has to be updated
	 */
	InputDeviceStateType();

	/**
	 * destructor - empty
	 */
	virtual ~InputDeviceStateType();
	
	/**
	 * checks if the scene should be updated
	 * @return updateScene
	 */
	bool shouldUpdateScene();
	
	/**
	 * miller has ended ops, set updateScene to false
	 */
	void signalMillingEnd();
	
	/**
	 * toggles updateScene
	 *
	 * @return !updateScene
	 */
	bool toggleUpdateScene();
};

#endif /* INPUTDEVICESTATETYPE_HPP_ */
