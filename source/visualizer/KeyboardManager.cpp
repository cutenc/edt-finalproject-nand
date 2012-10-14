/*
 * KeyboardManager.cpp
 *
 *  Created on: 10/ott/2012
 *      Author: alberto
 */

#include "KeyboardManager.hpp"

KeyboardManager::~KeyboardManager() {
	// TODO Auto-generated destructor stub
}

bool KeyboardManager::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa) {
   switch(ea.getEventType()) {
		case(osgGA::GUIEventAdapter::KEYDOWN): {
			switch(ea.getKey()) {
				case 's': 	// step
							std::cout << " move on!" << std::endl;
							idst->step = !idst->step;
							idst->stepForever = false;
							return false;

				case 'f':	// forever
							std::cout << "move on Forever!" << std::endl;
							idst->step = !idst->step;
							idst->stepForever = !idst->stepForever;
							return false;

				case 'p':	// pause
							std::cout << "pausa" << std::endl;
							if(idst->step) {
								idst->step = false;
								idst->stepForever = !idst->stepForever;
							} else {
								idst->step = true;
								idst->stepForever = !idst->stepForever;
							}
							return false;

//				case 'q':	//quit
//							std::cout << "ciao" << std::endl;

				default:
							return false;
			}
		break;
		}

	default:
				return false;
	}
}
