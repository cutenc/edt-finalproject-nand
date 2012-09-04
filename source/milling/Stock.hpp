/*
 * Stock.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef STOCK_HPP_
#define STOCK_HPP_

#include <ostream>

#include <boost/chrono.hpp>

#include <Eigen/Geometry>

#include "Cutter.hpp"
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
	
	u_long analyzed_leaves;
	u_long purged_leaves;
	
	/**
	 * counts the number of times a leaf has been deleted after more than
	 * one partial erosions (that is it has never been found totally inside
	 * the cutter).
	 */
	u_long lazy_purged_leaves;
	u_long pushed_leaves;
	u_long intersection_approx_errors;
	u_long intersection_approx_skips;
	
	boost::chrono::milliseconds elapsedTime;
	
	IntersectionResult() :
		waste(0), analyzed_leaves(0), purged_leaves(0), lazy_purged_leaves(0), 
		pushed_leaves(0), intersection_approx_errors(0), 
		intersection_approx_skips(0), elapsedTime(0)
	{ }
	
	bool isOutOfStock() const {
		return this->analyzed_leaves == this->intersection_approx_errors;
	}
	
	boost::chrono::microseconds meanTimePerLeaf() const {
		if (this->analyzed_leaves == 0)
			return boost::chrono::microseconds(0);
		
		boost::chrono::microseconds us = elapsedTime;
		return boost::chrono::microseconds(us.count() / this->analyzed_leaves);
	}
	
	friend std::ostream & operator<<(std::ostream &os, const IntersectionResult &res) {
		os << "\twaste: " << res.waste << std::endl
				<< "\t#analyzed leaves: " << res.analyzed_leaves << std::endl
				<< "\t#purged: " << res.purged_leaves << std::endl
				<< "\t#lazy purged: " << res.lazy_purged_leaves << std::endl
				<< "\t#pushed: " << res.pushed_leaves << std::endl
				<< "\t#approx_error: " << res.intersection_approx_errors << std::endl
				<< "\t#approx_skips: " << res.intersection_approx_skips << std::endl
				<< "\t#elapsed time: " << res.elapsedTime.count() / 1000.0 << "s" << std::endl
				<< "\t#mean time per leaf: " << res.meanTimePerLeaf().count() << "us" << std::endl
				<< "\toutOfStock?: " << res.isOutOfStock()
		;
		
		return os;
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
	
	IntersectionResult intersect(Cutter::CutterPtr cutter, const Eigen::Vector3d &traslation, const Eigen::Matrix3d &rotation);
	
private:
	
	VoxelInfo buildInfos(_Octree::LeafConstPtr leaf, 
			const Cutter::CutterPtr &cutter, const Eigen::Vector3d &traslation, const Eigen::Matrix3d &rotation);
	
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
	
	friend std::ostream & operator<<(std::ostream &os, const Stock &stock);
};


#endif /* STOCK_HPP_ */
