/*
 * SceneUpdater.cpp
 *
 *  Created on: 10/ott/2012
 *      Author: alberto
 */

#include "SceneUpdater.hpp"

#include <cassert>
#include <cmath>

#include <boost/date_time.hpp>

#include <osg/Geometry>
#include <osg/Projection>
#include <osg/MatrixTransform>

#include "common/constants.hpp"
#include "configuration/CNCMoveIterator.hpp"
#include "VisualizationUtils.hpp"

SceneUpdater::SceneUpdater(InputDeviceStateType::Ptr ids,
		MillingSignaler::Ptr signaler,
		const DisplayInfo &displayInfo,
		Stock::Ptr stock, Cutter::Ptr cutter) :
		
	IDST(ids), SIGNALER(signaler),
	stockRototras(new osg::PositionAttitudeTransform()),
	cutterRototras(new osg::PositionAttitudeTransform()),
	txtMoves(new osgText::Text), txtWaste(new osgText::Text),
	txtWater(new osgText::Text), stockPtr(stock), cutterPtr(cutter)

{
	// assign processers
	PROCESSERS[SignaledInfo::HAS_DATA] = &SceneUpdater::updateScene;
	PROCESSERS[SignaledInfo::MILLING_END] = &SceneUpdater::millingEnd;
	PROCESSERS[SignaledInfo::TIMEOUT] = &SceneUpdater::timeoutExpired;
	
	osg::ref_ptr< osg::Geode > axisGeode = new osg::Geode;
	axisGeode->addDrawable(VisualizationUtils::getAxis().get());
	
	// *** CUTTER SECTION ***
	/* prepare cutter scene tree that is made up of:
	 * - cutter Drawable (in position 0)
	 * - cutter axis
	 */
	// cutter meshing does not change during time but 
	cutterRototras->addChild(cutter->getMeshing()->getMesh().get());
	// prepare cutter axis (10% of the cutter max dimension)
	osg::PositionAttitudeTransform *pat = new osg::PositionAttitudeTransform;
	cutterRototras->addChild(pat);
	double cutterMin = cutterPtr->getBoundingBox().extents.minCoeff();
	double scaleFactor = cutterMin * 2;
	pat->setScale(osg::Vec3d(scaleFactor, scaleFactor, scaleFactor));
	pat->addChild(axisGeode.get());
	displayInfo.SON_OF_ROOT->addChild(cutterRototras);
	
	// *** STOCK SECTION ***
	/* prepare stock scene tree that is made up of:
	 * - stock drawable (in position 0)
	 * - stock axis
	 */
	/* adding an empty geode even to stock in order to not introduce more
	 * ifs to the callback operations
	 */
	stockRototras->addChild(new osg::Geode);
	// prepare stock axis (10% of the cutter max dimension)
	pat = new osg::PositionAttitudeTransform;
	stockRototras->addChild(pat);
	double stockMax = stockPtr->getExtents().maxCoeff();
	scaleFactor = stockMax * 0.1;
	pat->setScale(osg::Vec3d(scaleFactor, scaleFactor, scaleFactor));
	pat->addChild(axisGeode.get());
	displayInfo.SON_OF_ROOT->addChild(stockRototras);
	
	// *** TEXT SECTION ***
	/* creating a 2d-context and adding text to it */
	osg::Projection *twoD_context = new osg::Projection;
	twoD_context->setMatrix(osg::Matrix::ortho2D(0, displayInfo.winWidth,
			0, displayInfo.winHeight));
	
	osg::MatrixTransform *matT = new osg::MatrixTransform;
	matT->setMatrix(osg::Matrix::identity());
	matT->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	
	matT->addChild(twoD_context);
	displayInfo.SON_OF_ROOT->addChild(matT);
	
	/* adding text components to the scene
	 */
	osg::Geode *allTexts = new osg::Geode;
	txtMoves->setText("PROVA");
	txtMoves->setPosition(osg::Vec3(
			0,
			displayInfo.winHeight - 50,
			0
	));
	
	
	allTexts->addDrawable(txtMoves.get());
	allTexts->addDrawable(txtWaste.get());
	allTexts->addDrawable(txtWater.get());
	
	twoD_context->addChild(allTexts);
}

SceneUpdater::~SceneUpdater() {
}

void SceneUpdater::operator()(osg::Node* node, osg::NodeVisitor* nv) {
	static const boost::posix_time::milliseconds WAIT_TIME(1000 / FRAMES_PER_SECOND);
	
	//recupera l'oggetto cui fa riferimento la callback
	osg::ref_ptr<osg::Group> group = node->asGroup();
	assert(group);
	assert(group.get() == stockRototras->getParent(0));
	
	if(IDST->shouldUpdateScene()) {
		SignaledInfo infos = SIGNALER->awaitMiller(WAIT_TIME);
		
		u_char procIdx = static_cast< u_char >(infos.state);
		assert(procIdx < 3);
		(this->*(PROCESSERS[procIdx]))(infos);
	}
	
	// MUST BE CALLED to continue traversing
	traverse(node, nv);
}

void SceneUpdater::updateScene(const SignaledInfo& info) {
	assert(info.state == SignaledInfo::HAS_DATA);
	
	const CNCMove &move = info.lastMove;
	
	// build stock roto-traslation and update it
	stockRototras->setPosition(move.STOCK.TRASLATION.asOSG());
	stockRototras->setAttitude(move.STOCK.ROTATION.asOSG());
	
	// build cutter roto-traslation and update it
	cutterRototras->setPosition(move.CUTTER.TRASLATION.asOSG());
	cutterRototras->setAttitude(move.CUTTER.ROTATION.asOSG());
	
	// get & set the new stock mesh (removing previous one)
	Mesh::Ptr mesh = stockPtr->getMeshing();
	
	assert(stockRototras->getNumChildren() == 2);
	// will do a replace of the child in position 0
	stockRototras->setChild(0, mesh->getMesh().get());
	
	// TODO update text informations
//			SignaledInfo::MillingData::const_iterator it;
//			for (it = infos.millingResults->begin(); it != infos.millingResults->end(); ++it) {
//				std::cout << *it << std::endl;
//			}
	
}

void SceneUpdater::millingEnd(const SignaledInfo& info) {
	assert(info.state == SignaledInfo::MILLING_END);
	
	IDST->signalMillingEnd();
}

void SceneUpdater::timeoutExpired(const SignaledInfo& info) {
	assert(info.state == SignaledInfo::TIMEOUT);
	
	// DO NOTHING
}

