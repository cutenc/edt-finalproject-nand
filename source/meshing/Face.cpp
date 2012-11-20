/*
 * Face.cpp
 *
 *  Created on: 
 *      Author: socket
 */

#include "Face.hpp"

const Corner::CornerType Face::FACE_ADJACENCY[6][4] = {
	/* LEFT */ {Corner::BottomFrontLeft, Corner::UpperFrontLeft, Corner::UpperRearLeft, Corner::BottomRearLeft},
	/* FRONT */ {Corner::BottomFrontLeft, Corner::BottomFrontRight, Corner::UpperFrontRight, Corner::UpperFrontLeft},
	/* BOTTOM */ {Corner::BottomFrontLeft, Corner::BottomRearLeft, Corner::BottomRearRight, Corner::BottomFrontRight},
	/* RIGHT */ {Corner::BottomFrontRight, Corner::BottomRearRight, Corner::UpperRearRight, Corner::UpperFrontRight},
	/* REAR */ {Corner::BottomRearRight, Corner::BottomRearLeft, Corner::UpperRearLeft, Corner::UpperRearRight},
	/* TOP */ {Corner::UpperFrontLeft, Corner::UpperFrontRight, Corner::UpperRearRight, Corner::UpperRearLeft}
};
