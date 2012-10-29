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

KeyboardManager::~KeyboardManager() {
}

bool KeyboardManager::process(int key) {
	switch(key) {
		case '1': 	// step1
			CONTROLLER->stepOnce();
			break;
			
		case '2': 	// step10
			CONTROLLER->step(10u);
			break;
			
		case '3': 	// step50
			CONTROLLER->step(50u);
			break;
			
		case 'r':	// run
			CONTROLLER->play();
			break;
			
		case 'p':	// pause
			CONTROLLER->pause();
			break;
			
		case 't': // toggle visualization
			IDST->toggleUpdateScene();
			break;
			
		case 'k': // kill miller
			CONTROLLER->stop();
			break;
			
		case '?': // print help
		case 'h': // print help
			printHelp(std::cout);
			break;
			
		default:
			return false;
	}
	
	return true;
}

void KeyboardManager::printHelp(std::ostream& os) {
	os << "'r'\t\tRun milling as fast as possible" << std::endl
			<< "'1'\t\tStep position by position" << std::endl
			<< "'2'\t\tStep 10 positions at once" << std::endl
			<< "'3'\t\tStep 50 positions at once" << std::endl
			<< "'p'\t\tPause milling process" << std::endl
			<< "'t'\t\tToggle visualization updates (on/off)" << std::endl
			<< "'k'\t\tKill miller (stop milling)" << std::endl
			<< "'h'\t\tPrint help" << std::endl;
}

