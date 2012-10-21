/*
 * SceneUpdater.hpp
 *
 *  Created on: 10/ott/2012
 *      Author: alberto
 */

#ifndef SCENEUPDATER_HPP_
#define SCENEUPDATER_HPP_

#include <osg/NodeCallback>
#include <osg/PositionAttitudeTransform>

#include "InputDeviceStateType.hpp"
#include "MillingSignaler.hpp"

class SceneUpdater : public osg::NodeCallback {

private:
	typedef void (SceneUpdater::* SignalInfoProcesser)(const SignaledInfo &);
	
private:
	const InputDeviceStateType::Ptr IDST;
	const MillingSignaler::Ptr SIGNALER;
	const osg::ref_ptr< osg::PositionAttitudeTransform > stockRototras,
			cutterRototras;
	const Stock::Ptr stockPtr;
	const Cutter::Ptr cutterPtr;
	SignalInfoProcesser PROCESSERS[3];
	
public:
	SceneUpdater(InputDeviceStateType::Ptr ids,
			MillingSignaler::Ptr signaler,
			osg::ref_ptr< osg::Group > node,
			Stock::Ptr stock, Cutter::Ptr cutter);
	
	virtual ~SceneUpdater();
	
	// callback per l'aggiornamento della scena
	virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv );
	
private:
	void updateScene(const SignaledInfo &info);
	void millingEnd(const SignaledInfo &info);
	void timeoutExpired(const SignaledInfo &info);
};

#endif /* SCENEUPDATER_HPP_ */
