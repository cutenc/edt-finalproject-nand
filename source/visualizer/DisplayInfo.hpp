/**
 * @file DisplayInfo.hpp
 *
 * Created on: 22/ott/2012
 * Author: socket
 *
 * infos for the scene
 */

#ifndef DISPLAYINFO_HPP_
#define DISPLAYINFO_HPP_

#include <osg/Group>

struct DisplayInfo {
	/**
	 * constructor
	 *
	 * @param winWidth : width of the window
	 * @param winHeight : height of the window
	 */
	DisplayInfo(unsigned int winWidth, unsigned int winHeight) :
		winWidth(winWidth), winHeight(winHeight),
		ROOT(new osg::Group), SON_OF_ROOT(new osg::Group)
	{ }
	
	/**
	 * destructor
	 */
	virtual ~DisplayInfo() { }
	
	/**
	 * winWidth : width of the window
	 * winHeight : height of the window
	 */
	const unsigned int winWidth, winHeight;
	/** pointer to the root of the scene tree */
	const osg::ref_ptr< osg::Group > ROOT;
	/** pointer to the first object inserted into the scene tree, contains actual scene (stock, cutter) to be updated */
	const osg::ref_ptr< osg::Group > SON_OF_ROOT;
};


#endif /* DISPLAYINFO_HPP_ */
