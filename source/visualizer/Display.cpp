/*
 * Display.cpp
 *
 *  Created on: 27/set/2012
 *      Author: alberto
 */

#include "Display.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <osg/Group>
#include <osg/Array>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>

#include "common/Utilities.hpp"
#include "VisualizationUtils.hpp"
#include "InputDeviceStateType.hpp"
#include "KeyboardHandler.hpp"
#include "SceneUpdater.hpp"

Display::Display(
		Stock::Ptr stock, Cutter::Ptr cutter,
		MillingSignaler::Ptr signaler, SteppableController::Ptr millingCtrl) :
		stockPtr(stock), cutterPtr(cutter), signaler(signaler),
		controller(millingCtrl)
{
}

Display::~Display() {
}

//crea la scena e lancia il viewer
void Display::draw() {
	
	// get screen resolution
	osg::GraphicsContext::WindowingSystemInterface *wsi = osg::GraphicsContext::getWindowingSystemInterface();
	if (!wsi) {
		throw std::runtime_error("No windows system present");
	}
	
	u_int WIN_WIDTH, WIN_HEIGHT;
	wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), WIN_WIDTH, WIN_HEIGHT);
	
	DisplayInfo displayInfo(WIN_WIDTH, WIN_HEIGHT);
	
	// *** SON_OF_ROOT ***
	/* creo la classe che contiene le informazioni condivise tra SceneUpdater
	 * e KeyboardManager
	 */
	InputDeviceStateType::Ptr idst = boost::make_shared< InputDeviceStateType >();
	
	// creo il nodo che conterrà poi tutta la scena
	displayInfo.SON_OF_ROOT->setDataVariance(osg::Object::DYNAMIC);
	osg::ref_ptr< SceneUpdater > sceneUpd = new SceneUpdater(idst, signaler, displayInfo,
			stockPtr, cutterPtr); 
	displayInfo.SON_OF_ROOT->setUpdateCallback(sceneUpd.get());
	displayInfo.ROOT->addChild(displayInfo.SON_OF_ROOT.get());

	
	// *** LIGHT ***
//	osg::ref_ptr<osg::Light> light = osg::ref_ptr<osg::Light>(new osg::Light);
//	// each light must have a unique number
//	light->setLightNum(1);
//	osg::ref_ptr<osg::LightSource> ls = osg::ref_ptr<osg::LightSource>(new osg::LightSource);
//	ls->setLight(light);
//	osg::StateSet *stateSet = root->getOrCreateStateSet();
//	ls->setStateSetModes(*stateSet, osg::StateAttribute::ON);
//
//	light->setAmbient(osg::Vec4f(1.0, 1.0, 1.0, 1.0));
//	light->setDiffuse(osg::Vec4f(1.0, 1.0, 1.0, 1.0));
//	light->setSpecular(osg::Vec4f(1.0, 1.0, 1.0, 1.0));
//	light->setQuadraticAttenuation(0.1);
//
//	light->setPosition(osg::Vec4f(0.0, 0.0, 0.0, 1.0));
//	ls->setLocalStateSetModes(osg::StateAttribute::ON);
//	
//	// aggiungo la sonOfRoot alla luce e la luce a root
//	ls->addChild(sonOfRoot);
//	
//	displayInfo.ROOT->getOrCreateStateSet()->setMode(
//			GL_LIGHTING,
//			osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE
//	);
	
	
	// *** WORLD AXIS ***
	osg::PositionAttitudeTransform *axisPAT = new osg::PositionAttitudeTransform;
	double axisScaleFactor = 50;
	axisPAT->setScale(osg::Vec3d(axisScaleFactor, axisScaleFactor, axisScaleFactor));
	osg::Geode* axisGeode = new osg::Geode();
	axisPAT->addChild(axisGeode);
	axisGeode->addDrawable( VisualizationUtils::getAxis().get() );
	displayInfo.ROOT->addChild(axisPAT);
	
	
	// *** VIEWER ***
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	viewer->setUpViewOnSingleScreen(0);
//	viewer->setUpViewInWindow(0, 0, displayInfo.winWidth, displayInfo.winHeight, 0);
	
	osgGA::TrackballManipulator *tbmp = new osgGA::TrackballManipulator;
	osg::Vec3d initPos = GeometryUtils::toOsg(stockPtr->getExtents() * 3);
	tbmp->setHomePosition(initPos, osg::Vec3d(), osg::Z_AXIS, false);
	viewer->setCameraManipulator(tbmp);
	
	osg::ref_ptr<KeyboardHandler> kh = new KeyboardHandler(idst, controller);
	viewer->addEventHandler(kh);
	// setting up scene and realize
	viewer->setSceneData(displayInfo.ROOT.get());
	viewer->realize();

	viewer->run();
}
