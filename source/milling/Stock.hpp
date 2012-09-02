/*
 * Stock.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef STOCK_HPP_
#define STOCK_HPP_

#include <Eigen/Geometry>

#include "cutters.hpp"
#include "VoxelInfo.hpp"
#include "Octree.hpp"
#include "SimpleBox.hpp"
#include "configuration/StockDescription.hpp"

struct IntersectionResult {
	
	/**
	 * Gives an estimation of how much volume has been removed during the
	 * milling operation
	 */
	double waste;
	
	/**
	 * Tells wether cutter were totally out of the stock (no intersection) or
	 * not.
	 */
	bool outOfStock;
};

class Stock {
	
	const u_int MAX_DEPTH;
	const Eigen::Vector3d EXTENT;
	const Eigen::Vector3d STOCK_MODEL_TRASLATION;
	Octree< VoxelInfo > MODEL;
	
public:
	Stock(const StockDescription &desc, u_int maxDepth);
	virtual ~Stock();
	
	IntersectionResult intersect(CutterPtr cutter, const Eigen::Vector3d &traslation, const Eigen::Matrix3d &rotation);
	
private:
	
	VoxelInfo buildInfos(Octree< VoxelInfo >::LeafConstPtr leaf, 
			const CutterPtr &cutter, const Eigen::Vector3d &traslation, const Eigen::Matrix3d &rotation);
	
	/**
	 * 
	 * @param voxel
	 * @param oldInfo
	 * @param updatedInfo that is old VoxelInfo after they have been updated
	 * with new ones
	 * @return
	 */
	double getApproxWaste(const SimpleBox &voxel, const VoxelInfo &oldInfo, const VoxelInfo &updatedInfo) const;
	
	double intersectedVolume(const SimpleBox &voxel, const VoxelInfo &info) const;
};


#endif /* STOCK_HPP_ */
