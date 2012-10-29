/*
 * KeyboardHandler.cpp
 *
 *  Created on: 10/ott/2012
 *      Author: alberto
 */

#include "KeyboardHandler.hpp"

#include <iostream>

KeyboardHandler::KeyboardHandler(InputDeviceStateType::Ptr ids, SteppableController::Ptr controller) :
	manager(ids, controller)
{ }

KeyboardHandler::~KeyboardHandler() { }

bool KeyboardHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&) {
	if (ea.getEventType() != osgGA::GUIEventAdapter::KEYDOWN) {
		return false;
	}
	
	return manager.process(ea.getKey());
}
