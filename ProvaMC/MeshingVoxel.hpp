/*
 * MeshingVoxel.hpp
 *
 *  Created on: 22/ott/2012
 *      Author: alberto
 */

#ifndef MESHINGVOXEL_HPP_
#define MESHINGVOXEL_HPP_

#include <eigen3/Eigen/Geometry>

class MeshingVoxel {
public:

	MeshingVoxel(ShiftedBox::Ptr sbp, double *weights) {

		int i;

		this->weights = weights;

		for (i = 0; i < 8; i++) {
			Eigen::Vector3d corner = sbp.get()->getCorner(i, sbp.get()->getShift());
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
