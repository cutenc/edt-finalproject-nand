/*
 * InputDeviceStateType.cpp
 *
 *  Created on: 10/ott/2012
 *      Author: alberto
 */

#include "InputDeviceStateType.hpp"




InputDeviceStateType::InputDeviceStateType() : updateScene(true)
{
}

InputDeviceStateType::~InputDeviceStateType() {
}

bool InputDeviceStateType::shouldUpdateScene() {
	// TODO
	return updateScene;
}

void InputDeviceStateType::signalMillingEnd() {
	updateScene = false;
}

bool InputDeviceStateType::toggleUpdateScene() {
	return updateScene = !updateScene;
}



