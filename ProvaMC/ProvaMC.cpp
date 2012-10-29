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

//	for (i = 0; i < asdSize; i++) {
//
//		vd = asd->at(i).getData();
//
//		for (j = 0; j < vd.size(); j++) {
//			sb = vd[i].first;
//			v  = vd[i].second;
//
//			for (k = 0; k < 8; k++) {
//				Eigen::Vector3d corners = sb.simpleBox.get()->getCorner(k);
//
//			}
//
//		}
//
//	}

	/*
	 * step 1: crea VoxelInfo
	 */
	VoxelInfo *viArray = VoxelInfo[nVoxels];
	for (i = 0; i < nVoxels; i++) {
		viArray[i] = new VoxelInfo(VoxelInfo::DEFAULT_INSIDENESS());
	}

	/*
	 * step 2: crea ShiftedBoxes
	 */
	ShiftedBox *vArray = ShiftedBox[nVoxels];
	for (i = 0; i < nVoxels; i++) {
		SimpleBox::Ptr sbp = boost::make_shared<SimpleBox>(Eigen::Vector3d(2,2,2));
		ShiftedBox::Translation3ld shift(2*i,0,0);
		vArray[i] = new ShiftedBox(sbp, shift);
	}

	/*
	 * step 3: crea MeshingVoxel
	 */
	MeshingVoxel *mv = MeshingVoxel[nVoxels];
	for (i = 0; i < nVoxels; i++) {
		mv[i] = MeshingVoxel(						// <----- here we are
					vArray[i].simpleBox,
					this->getDistance(				// <----- here it is
							Eigen::Vector3d(0,0,0),
							vArray[i].simpleBox
					)
				);
	}

	/*
	 * step 4: chiama MarchingCubes
	 */
	resultGroup = osg::ref_ptr<osg::Group>(new osg::Group);
	for (i = 0; i < nVoxels; i++) {
		osg::ref_ptr<osg::Geode> geode = osg::ref_ptr<osg::Geode>(new osg::Geode);
		geode->addDrawable(Meshing::buildMesh(mv[i], MC_THRESHOLD));
		resultGroup->addChild(geode);
	}

}

ProvaMC::~ProvaMC() {};



double* ProvaMC::getDistance(const Eigen::Vector3d point, SimpleBox::Ptr sbp) {
//
//	if (point[2] < 0)
//			return boost::math::changesign(CommonUtils::INFINITE());
//
//	double halfDim = sbp.get()->getHalfExtent();
//
//
//	/* following implicit function given in "Adaptive NC Simulation
//	 * for Multi-axis Solid Machining" for a flat endmill aligned with
//	 * Z-axis we can write:
//	 */
//	double firstTerm = fabs((double)point[2] - halfDim) - halfDim;
//
//	/* in order to reduce floating point approximation errors we will
//	 * rewrite
//	 * double secondTerm = sqr(point.getX()) + sqr(point.getY()) - SQR_RADIUS;
//	 * in a more floating-point-friendly way.
//	 *
//	 * http://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
//	 */
//	double secondTerm = boost::math::pow< 2 >((double)point[0])
//			+ (point[1] + RADIUS) * (point[1] - RADIUS);
//
//	double distance = fmax(firstTerm, secondTerm);
//
//	/* I have to add a minus sign because in paper specification
//	 * distance <0 means "inside surface" and >0 "outside surface" but,
//	 * in our specification <0 means "outside" and >0 "inside"
//	 */
//	return boost::math::changesign(distance);
//
//
//	/*********************************  ?????  **************************************/
//
	double weights[8] = {
			VoxelInfo::DEFAULT_INSIDENESS(),VoxelInfo::DEFAULT_INSIDENESS(),
			VoxelInfo::DEFAULT_INSIDENESS(),VoxelInfo::DEFAULT_INSIDENESS(),
			VoxelInfo::DEFAULT_INSIDENESS(),VoxelInfo::DEFAULT_INSIDENESS(),
			VoxelInfo::DEFAULT_INSIDENESS(),VoxelInfo::DEFAULT_INSIDENESS()
	};
//
////	if (point.getX() == halfDim || point.getY() == halfDim || point.getZ() == halfDim) {
////		weights = {0,0,0,0,0,0,0,0};
////	} else {
//////		if () {
//////		}
////	}
//
	int i;
	for (i = 0; i < 8; i++) {
		if (point[2] < 0)
				return boost::math::changesign(CommonUtils::INFINITE());

		double halfDim = sbp.get()->getHalfExtent();


		/* following implicit function given in "Adaptive NC Simulation
		 * for Multi-axis Solid Machining" for a flat endmill aligned with
		 * Z-axis we can write:
		 */
		double firstTerm = fabs((double)point[2] - halfDim) - halfDim;

		/* in order to reduce floating point approximation errors we will
		 * rewrite
		 * double secondTerm = sqr(point.getX()) + sqr(point.getY()) - SQR_RADIUS;
		 * in a more floating-point-friendly way.
		 *
		 * http://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
		 */
		double secondTerm = boost::math::pow< 2 >((double)point[0])
				+ (point[1] + RADIUS) * (point[1] - RADIUS);

		double distance = fmax(firstTerm, secondTerm);

		/* I have to add a minus sign because in paper specification
		 * distance <0 means "inside surface" and >0 "outside surface" but,
		 * in our specification <0 means "outside" and >0 "inside"
		 */
		//return
		weights[i] = boost::math::changesign(distance);
	}

	return weights;
}
