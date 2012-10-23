/*
 * DisplayInfo.hpp
 *
 *  Created on: 22/ott/2012
 *      Author: socket
 */

#ifndef DISPLAYINFO_HPP_
#define DISPLAYINFO_HPP_

#include <osg/Group>

struct DisplayInfo {
	DisplayInfo(u_int winWidth, u_int winHeight) :
		winWidth(winWidth), winHeight(winHeight),
		ROOT(new osg::Group), SON_OF_ROOT(new osg::Group)
	{ }
	
	virtual ~DisplayInfo() { }
	
	const u_int winWidth, winHeight;
	const osg::ref_ptr< osg::Group > ROOT;
	const osg::ref_ptr< osg::Group > SON_OF_ROOT;
};


#endif /* DISPLAYINFO_HPP_ */
