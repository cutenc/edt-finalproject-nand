/*
 * ProvaMC.cpp
 *
 *  Created on: 17/ott/2012
 *      Author: alberto
 */

#include "ProvaMC.hpp"

ProvaMC::ProvaMC(boost::array<StoredData, asdSize> *asd) {

	int i, j, k;
	int nVoxels = 10;
	StoredData sd;
	VoxelInfo v;
	ShiftedBox sb;
	StoredData::VoxelData vd;

//	VoxelInfo *viList = VoxelInfo[nVoxels];
//
//	for (i = 0; i < nVoxels; i++) {
//		viList[i] = new VoxelInfo(VoxelInfo::DEFAULT_INSIDENESS());
//	}

	for (i = 0; i < asdSize; i++) {

		vd = asd->at(i).getData();

		for (j = 0; j < vd.size(); j++) {
			sb = vd[i].first;
			v  = vd[i].second;


			for (k = 0; k < 8; k++) {
				Eigen::Vector3d corners = sb.getCorner(k, sb.getShift());

			}

		}

	}

}

ProvaMC::~ProvaMC() {};



double* ProvaMC::distance(Point3D point, SimpleBox::Ptr sbp) {

	double weights[8] = {
			VoxelInfo::DEFAULT_INSIDENESS(),VoxelInfo::DEFAULT_INSIDENESS(),
			VoxelInfo::DEFAULT_INSIDENESS(),VoxelInfo::DEFAULT_INSIDENESS(),
			VoxelInfo::DEFAULT_INSIDENESS(),VoxelInfo::DEFAULT_INSIDENESS(),
			VoxelInfo::DEFAULT_INSIDENESS(),VoxelInfo::DEFAULT_INSIDENESS()
	};

	double halfDim = sbp.get()->getHalfExtent();


	if (point.getX() == halfDim || point.getY() == halfDim || point.getZ() == halfDim) {
		weights = {0,0,0,0,0,0,0,0};
	} else {
//		if () {
//		}
	}

	return weights;
}
