/*
 * Display.hpp
 *
 *  Created on: 27/set/2012
 *      Author: alberto
 */

#ifndef DISPLAY_HPP_
#define DISPLAY_HPP_

#include "threading/SteppableController.hpp"
#include "milling/Stock.hpp"
#include "milling/Cutter.hpp"
#include "MillingSignaler.hpp"
#include "DisplayInfo.hpp"

class Display {
	
private:
	const Stock::Ptr stockPtr;
	const Cutter::Ptr cutterPtr;
	const MillingSignaler::Ptr signaler;
	const SteppableController::Ptr controller;
	
public:
	Display(Stock::Ptr stock, Cutter::Ptr cutter,
			MillingSignaler::Ptr signaler,
			SteppableController::Ptr millingCtrl);
	
	virtual ~Display();

	void draw();
};

#endif /* DISPLAY_HPP_ */
