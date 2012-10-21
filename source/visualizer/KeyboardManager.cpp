/*
 * KeyboardManager.cpp
 *
 *  Created on: 10/ott/2012
 *      Author: alberto
 */

#include "KeyboardManager.hpp"

#include <iostream>

KeyboardManager::KeyboardManager(InputDeviceStateType::Ptr ids, SteppableController::Ptr controller) :
	IDST(ids), CONTROLLER(controller)
{ }

KeyboardManager::~KeyboardManager() { }

bool KeyboardManager::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
	if (ea.getEventType() != osgGA::GUIEventAdapter::KEYDOWN) {
		return false;
	}
	
	switch(ea.getKey()) {
		case '1': 	// step
			CONTROLLER->stepOnce();
			break;
		case '2': 	// step
			CONTROLLER->step(10u);
			break;
		case '3': 	// step
			CONTROLLER->step(50u);
			break;
		case 'r':	// run
			CONTROLLER->play();
			break;
		case 'p':	// pause
			CONTROLLER->pause();
			break;
			
		default:
			return false;
	}
	
	return true;
}

