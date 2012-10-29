/*
 * ProvaMC.hpp
 *
 *  Created on: 17/ott/2012
 *      Author: alberto
 */

#ifndef PROVAMC_HPP_
#define PROVAMC_HPP_

#include <boost/array.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

#include <math.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>

#include "MeshingVoxel.hpp"
#include "Point3D.hpp"
#include "SimpleBox.hpp"
#include "ShiftedBox.hpp"
#include "StoredData.hpp"
#include "VoxelInfo.hpp"

class ProvaMC {
public:

	const int MAX_WEIGHT = 1000;
	const double MC_THRESHOLD = 0.0;
	const double RADIUS = 0.5;
	int asdSize;
	osg::ref_ptr<osg::Group> resultGroup;

	osg::ref_ptr<osg::Group> getResultGroup() const {
		return resultGroup;
	}

	ProvaMC(boost::array<StoredData, asdSize> *asd);
	virtual ~ProvaMC();

	double* getDistance(const Eigen::Vector3d point, SimpleBox::Ptr sbp);

};

#endif /* PROVAMC_HPP_ */
