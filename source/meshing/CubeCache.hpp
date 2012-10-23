/*
 * CubeCache.hpp
 *
 *  Created on: 22/ott/2012
 *      Author: socket
 */

#ifndef CUBECACHE_HPP_
#define CUBECACHE_HPP_

#include <map>

#include <osg/ShapeDrawable>

#include "milling/SimpleBox.hpp"

class CubeCache {
	
private:
	typedef std::map<const SimpleBox *, osg::ref_ptr< osg::ShapeDrawable > > CacheType;
	
private:
	CacheType CACHE;
	
public:
	CubeCache();
	virtual ~CubeCache();
	
	osg::ref_ptr< osg::ShapeDrawable > getCube(const SimpleBox::ConstPtr &ptr);
};

#endif /* CUBECACHE_HPP_ */
