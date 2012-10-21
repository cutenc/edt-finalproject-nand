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

#include "VisualizationUtils.hpp"
#include "InputDeviceStateType.hpp"
#include "KeyboardManager.hpp"
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

	osg::ref_ptr<osg::Group> root = osg::ref_ptr<osg::Group>(new osg::Group);

	// creo il nodo che conterrà poi tutta la scena
	osg::ref_ptr<osg::Group> sonOfRoot = osg::ref_ptr<osg::Group>(new osg::Group);
	
	/* creo la classe che contiene le informazioni condivise tra SceneUpdater
	 * e KeyboardManager
	 */
	InputDeviceStateType::Ptr idst = boost::make_shared< InputDeviceStateType >();
	std::cout << "input device state type created" << std::endl;
	
	osg::ref_ptr<KeyboardManager> kh = new KeyboardManager(idst, controller);
	std::cout << "keyboard handler created" << std::endl;
	
	sonOfRoot->setDataVariance(osg::Object::DYNAMIC);
	osg::ref_ptr< SceneUpdater > sceneUpd = new SceneUpdater(idst, signaler, sonOfRoot,
			stockPtr, cutterPtr); 
	sonOfRoot->setUpdateCallback(sceneUpd.get());

	std::cout << "Creo il viewer il viewer... ";
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	std::cout << "fatto" << std::endl;

	osgGA::TrackballManipulator *tbmp = new osgGA::TrackballManipulator;
	viewer->setCameraManipulator(tbmp);
	tbmp->setHomePosition(osg::Vec3d(400, 400, 400), osg::Vec3d(), osg::Z_AXIS, false);
	
	viewer->setUpViewInWindow(128, 32, 1024, 768, 0);
	viewer->addEventHandler(kh);

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
	
	root->addChild(sonOfRoot.get());

	root->getOrCreateStateSet()->setMode(
			GL_LIGHTING,
			osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE
	);

	osg::PositionAttitudeTransform *axisPAT = new osg::PositionAttitudeTransform;
	root->addChild(axisPAT);
	double axisScaleFactor = 50;
	axisPAT->setScale(osg::Vec3d(axisScaleFactor, axisScaleFactor, axisScaleFactor));
	osg::Geode* axisGeode = new osg::Geode();
	axisPAT->addChild(axisGeode);
	axisGeode->addDrawable( VisualizationUtils::getAxis().get() );

	viewer->setSceneData( root );
	
	viewer->realize();

	std::cout << "Run viewer" << std::endl;

	viewer->run();
}


