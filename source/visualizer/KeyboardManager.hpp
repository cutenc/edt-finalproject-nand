/*
 * KeyboardManager.hpp
 *
 *  Created on: 10/ott/2012
 *      Author: alberto
 */

#ifndef KEYBOARDMANAGER_HPP_
#define KEYBOARDMANAGER_HPP_

class KeyboardManager : public osgGA::GUIEventHandler {
public:
	KeyboardManager(InputDeviceStateType* ids) {
		std::cout << "d" << std::endl;

		idst = ids;
		std::cout << "e" << std::endl;
	}
	virtual ~KeyboardManager();

	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&);
	virtual void accept(osgGA::GUIEventHandlerVisitor& v)   { v.visit(*this); };
protected:
	InputDeviceStateType* idst;
};

#endif /* KEYBOARDMANAGER_HPP_ */
