/*
 * InputDeviceStateType.hpp
 *
 *  Created on: 10/ott/2012
 *      Author: alberto
 */

#ifndef INPUTDEVICESTATETYPE_HPP_
#define INPUTDEVICESTATETYPE_HPP_

class InputDeviceStateType {
public:
	InputDeviceStateType():
	      step(false) , stepForever(false) {
			std::cout << "InputDeviceStateType creato" << std::endl;
		}
	virtual ~InputDeviceStateType();

   bool step;
   bool stepForever;


};

#endif /* INPUTDEVICESTATETYPE_HPP_ */
