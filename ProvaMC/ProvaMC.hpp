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

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "MeshingVoxel.hpp"
#include "Point3D.hpp"
#include "SimpleBox.hpp"
#include "StoredData.hpp"
#include "VoxelInfo.hpp"

class ProvaMC {
public:

	const int MAX_WEIGHT = 1000;
	int asdSize;

	ProvaMC(boost::array<StoredData, asdSize> *asd);
	virtual ~ProvaMC();

	double* distance(Point3D point, SimpleBox::Ptr sbp);

};

#endif /* PROVAMC_HPP_ */
