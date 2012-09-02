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
	 * counts the number of times a leaf has been deleted after more than
	 * one partial erosions (that is it has never been found totally inside
	 * the cutter).
	 */
	u_int lazy_leaf_purging;
	u_int analyzed_leaves;
	u_int purged_leaves;
	u_int pushed_leaves;
	u_int intersection_approx_skips;
	u_int intersection_approx_errors;
	
	IntersectionResult() :
		waste(0), analyzed_leaves(0), purged_leaves(0),
		pushed_leaves(0), intersection_approx_errors(0),
		intersection_approx_skips(0), lazy_leaf_purging(0)
	{ }
	
	bool isOutOfStock() const {
		return this->analyzed_leaves == this->intersection_approx_errors;
	}
};

class Stock {
	
	typedef Octree< VoxelInfo > _Octree;
	
	const u_int MAX_DEPTH;
	const Eigen::Vector3d EXTENT;
	const Eigen::Vector3d STOCK_MODEL_TRASLATION;
	_Octree MODEL;
	
public:
	Stock(const StockDescription &desc, u_int maxDepth);
	virtual ~Stock();
	
	IntersectionResult intersect(CutterPtr cutter, const Eigen::Vector3d &traslation, const Eigen::Matrix3d &rotation);
	
private:
	
	VoxelInfo buildInfos(_Octree::LeafConstPtr leaf, 
			const CutterPtr &cutter, const Eigen::Vector3d &traslation, const Eigen::Matrix3d &rotation);
	
	/**
	 * Updates waste on given \c leaf according to given \c newInfo and then
	 * returns an approximation of the amount of new waste.
	 * 
	 * @param leaf
	 * @param newInfo
	 * @return
	 */
	double updateWaste(_Octree::LeafPtr leaf, const VoxelInfo &newInfo);
	
	/**
	 * 
	 * @param voxel
	 * @param oldInfo
	 * @param updatedInfo that is old VoxelInfo after they have been updated
	 * with new ones
	 * @return
	 */
	double getApproxWaste(const SimpleBox &box, const VoxelInfo &oldInfo, const VoxelInfo &updatedInfo) const;
	
	double intersectedVolume(const SimpleBox &box, const VoxelInfo &info) const;
	
	bool canPushLevel(_Octree::LeafPtr leaf) const;
};


#endif /* STOCK_HPP_ */
