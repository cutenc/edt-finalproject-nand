/**
 * @file KeyboardManager.hpp
 *
 * Created on: 10/ott/2012
 * Author: alberto
 *
 * recognizes which key has been pressed, and reacts accordingly.
 * Is controlled by KeyboardHandler
 */

#ifndef KEYBOARDMANAGER_HPP_
#define KEYBOARDMANAGER_HPP_

#include <ostream>

#include "InputDeviceStateType.hpp"
#include "threading/SteppableController.hpp"

class KeyboardManager {
	
private:
	/** needed by the scene updater, checks if the scene has to be updated */
	const InputDeviceStateType::Ptr IDST;
	/** keeps track of milling ops */
	const SteppableController::Ptr CONTROLLER;
	
public:
	/**
	 * constructor
	 *
	 * @param ids : needed by the scene updater, checks if the scene has to be updated
	 * @param controller : keeps track of milling ops
	 */
	KeyboardManager(InputDeviceStateType::Ptr ids, SteppableController::Ptr controller);

	/**
	 * destructor - empty
	 */
	virtual ~KeyboardManager();
	
	/**
	 * performs the right action, according to the key pressed
	 *
	 * only a limited set of keys are valid in this program (see the helper function),
	 * and all of them are processed by this function, setting the right parameters of the controller
	 *
	 * @param key : the key pressed by the user
	 * @return true if given key has been correctly processed, false otherwise
	 */
	bool process(int key);
	
	/**
	 * the helper displaying the valid keys, and the actions they perform
	 *
	 * @param stream : the output stream to use
	 */
	void printHelp(std::ostream &stream);
};

#endif /* KEYBOARDMANAGER_HPP_ */
