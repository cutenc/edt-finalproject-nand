/*
 * KeyboardHandler.hpp
 *
 *  Created on: 26/ott/2012
 *      Author: socket
 */

#ifndef KEYBOARDHANDLER_HPP_
#define KEYBOARDHANDLER_HPP_

#include <osgGA/GUIEventHandler>

#include "KeyboardManager.hpp"

class KeyboardHandler : public osgGA::GUIEventHandler {
	
private:
	KeyboardManager manager;
	
public:
	KeyboardHandler(InputDeviceStateType::Ptr ids, SteppableController::Ptr controller);
	virtual ~KeyboardHandler();

	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&);	
};


#endif /* KEYBOARDHANDLER_HPP_ */
