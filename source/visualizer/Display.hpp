/*
 * Display.hpp
 *
 *  Created on: 27/set/2012
 *      Author: alberto
 */

#ifndef DISPLAY_HPP_
#define DISPLAY_HPP_

#include <osg/Billboard>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Material>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/PolygonMode>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/PositionAttitudeTransform>
#include <osgGA/GUIEventHandler>
#include <osgGA/TrackballManipulator>
#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/TexEnv>
#include <osg/TexGen>
#include <osg/ShapeDrawable>
#include <iostream>

#include "common/meshes.hpp"
#include "visualizer/SceneUpdater.hpp"

class Display {
public:
	Display();
	virtual ~Display();

	void draw(boost::shared_ptr<StockMesh>);

protected:
	osg::Drawable* createSquare(const osg::Vec3& corner,const osg::Vec3& width,const osg::Vec3& height, osg::Image* image=NULL);
	osg::Drawable* createAxis(const osg::Vec3& corner,const osg::Vec3& xdir,const osg::Vec3& ydir,const osg::Vec3& zdir);
};

#endif /* DISPLAY_HPP_ */
