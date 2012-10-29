/*
 * MeshingVoxel.hpp
 *
 *  Created on: 22/ott/2012
 *      Author: alberto
 */

#ifndef MESHINGVOXEL_HPP_
#define MESHINGVOXEL_HPP_

#include <Eigen/Geometry>

#include "Voxel.hpp"

class MeshingVoxel {
public:

	MeshingVoxel(ShiftedBox::Ptr sbp, double *weights) {

		this->weights = weights;

		Voxel::CornerTypeArray &order = Voxel::DEFAULT_ORDER;

		u_int i = 0;
		Voxel::CornerTypeArray::const_iterator it;
		for (it = order.begin(); it != order.end(); ++it) {
			Eigen::Vector3d corner = sbp.get()->simpleBox.get()->getCorner(*it);
			corners->push_back(osg::Vec3f(corner[0], corner[1], corner[2]));
		}
	}

	virtual ~MeshingVoxel();

	osg::ref_ptr<osg::Vec3Array> getCorners() const {
		return corners;
	}

	void setCorners(osg::ref_ptr<osg::Vec3Array> corners) {
		this->corners = corners;
	}

	double* getWeights() const {
		return weights;
	}

	void setWeights(double* weights) {
		this->weights = weights;
	}

private:
	osg::ref_ptr<osg::Vec3Array> corners;
	double *weights;

};

#endif /* MESHINGVOXEL_HPP_ */
