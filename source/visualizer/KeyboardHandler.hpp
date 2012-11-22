/**
 * @file KeyboardHandler.hpp
 *
 * Created on: 26/ott/2012
 * Author: socket
 *
 * recognizes keyboard events
 */

#ifndef KEYBOARDHANDLER_HPP_
#define KEYBOARDHANDLER_HPP_

#include <osgGA/GUIEventHandler>

#include "KeyboardManager.hpp"

class KeyboardHandler : public osgGA::GUIEventHandler {
	
private:
	/** the keyboard manager, which will manage the event */
	KeyboardManager manager;
	
public:
	/**
	 * constructor
	 *
	 * @param ids : needed by the scene updater, checks if the scene has to be updated
	 * @param controller : keeps track of milling ops
	 */
	KeyboardHandler(InputDeviceStateType::Ptr ids, SteppableController::Ptr controller);

	/**
	 * destructor - empty
	 */
	virtual ~KeyboardHandler();

	/**
	 * recognizes if an event is happened and calls the KeyboardManager process routine to perform the right action
	 *
	 * @param ea : event
	 * @return false if no valid event is happened, else returns the result of the action performed by the KeyboardManager
	 */
	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&);	
};


#endif /* KEYBOARDHANDLER_HPP_ */
