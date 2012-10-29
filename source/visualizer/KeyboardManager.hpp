/*
 * KeyboardManager.hpp
 *
 *  Created on: 10/ott/2012
 *      Author: alberto
 */

#ifndef KEYBOARDMANAGER_HPP_
#define KEYBOARDMANAGER_HPP_

#include <ostream>

#include "InputDeviceStateType.hpp"
#include "threading/SteppableController.hpp"

class KeyboardManager {
	
private:
	const InputDeviceStateType::Ptr IDST;
	const SteppableController::Ptr CONTROLLER;
	
public:
	KeyboardManager(InputDeviceStateType::Ptr ids, SteppableController::Ptr controller);
	virtual ~KeyboardManager();
	
	/**
	 * 
	 * @param key
	 * @return \c true if given key has been processed, false otherwise
	 */
	bool process(int key);
	
	void printHelp(std::ostream &stream);
};

#endif /* KEYBOARDMANAGER_HPP_ */
