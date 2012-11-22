/**
 * @file Display.hpp
 *
 * Created on: 27/set/2012
 * Author: alberto
 *
 * creates che viewer and renders the scene
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
	/** pointer to the Stock object */
	const Stock::Ptr stockPtr;
	/** pointer to the Cutter object */
	const Cutter::Ptr cutterPtr;
	/** pointer to the MillingSignaler object */
	const MillingSignaler::Ptr signaler;
	/** pointer to the SteppableController object */
	const SteppableController::Ptr controller;
	
public:
	/**
	 * constructor
	 *
	 * @param stock : pointer to the Stock object
	 * @param cutter : pointer to the Cutter object
	 * @param signaler : pointer to the MillingSignaler object
	 * @param millingCtrl : pointer to the SteppableController object
	 */
	Display(Stock::Ptr stock, Cutter::Ptr cutter,
			MillingSignaler::Ptr signaler,
			SteppableController::Ptr millingCtrl);
	
	virtual ~Display();

	/**
	 * creates the scene and renders it
	 *
	 * no parameters needed, recognizes the screen characteristics, creates the OSG Viewer object,
	 * creates the scene tree and renders it, running until the window in closed
	 */
	void draw();
};

#endif /* DISPLAY_HPP_ */
