/*
 * KeyboardManager.hpp
 *
 *  Created on: 10/ott/2012
 *      Author: alberto
 */

#ifndef KEYBOARDMANAGER_HPP_
#define KEYBOARDMANAGER_HPP_

#include <osgGA/GUIEventHandler>

#include "InputDeviceStateType.hpp"
#include "threading/SteppableController.hpp"

class KeyboardManager : public osgGA::GUIEventHandler {
	
private:
	const InputDeviceStateType::Ptr IDST;
	const SteppableController::Ptr CONTROLLER;
	
public:
	KeyboardManager(InputDeviceStateType::Ptr ids, SteppableController::Ptr controller);
	virtual ~KeyboardManager();

	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&);	
};

#endif /* KEYBOARDMANAGER_HPP_ */
