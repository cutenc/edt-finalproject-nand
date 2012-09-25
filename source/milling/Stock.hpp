/*
 * Stock.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef STOCK_HPP_
#define STOCK_HPP_

#include <ostream>
#include <sstream>

#include <boost/chrono.hpp>
#include <boost/shared_ptr.hpp>

#include <Eigen/Geometry>

#include "common/Model3D.hpp"
#include "configuration/StockDescription.hpp"
#include "meshing/Mesher.hpp"
#include "Cutter.hpp"
#include "VoxelInfo.hpp"
#include "Octree.hpp"
#include "SimpleBox.hpp"

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
	
	u_long updated_data_leaves;
	
	/**
	 * counts number of leaves that bounding-box intersection test states are 
	 * intersecting but, at the end, distance misuration
	 * finds no corner inside cutter blade. In other words it counts the
	 * number of leaves that has been analyzed but the corners of which were
	 * always outside cutter blade (it may happen even for a too shallow
	 * depth limit)  
	 */
	u_long intersection_approx_errors;
	
	u_long intersection_approx_skips;
	
	boost::chrono::milliseconds elapsedTime;
	
	IntersectionResult() :
		waste(0), analyzed_leaves(0), purged_leaves(0), lazy_purged_leaves(0), 
		pushed_leaves(0), updated_data_leaves(0), intersection_approx_errors(0), 
		intersection_approx_skips(0), elapsedTime(0)
	{ }
	
	boost::chrono::microseconds meanTimePerLeaf() const {
		if (this->analyzed_leaves == 0)
			return boost::chrono::microseconds(0);
		
		boost::chrono::microseconds us = elapsedTime;
		return boost::chrono::microseconds(us.count() / this->analyzed_leaves);
	}
	
	static std::string getPrintHeader() {
		std::stringstream ss;
		ss << "waste"
				<< "\t#analyzed leaves"
				<< "\t#purged"
				<< "\t#lazy purged"
				<< "\t#updated_data_leaves"
				<< "\t#pushed"
				<< "\t#approx_error"
				<< "\t#approx_skips"
				<< "\t#elapsed time (s)"
				<< "\t#mean time per leaf (us)";
		return ss.str();
	}
	
	friend std::ostream & operator<<(std::ostream &os, const IntersectionResult &res) {
		os << res.waste
				<< "\t" << res.analyzed_leaves
				<< "\t" << res.purged_leaves
				<< "\t" << res.lazy_purged_leaves
				<< "\t" << res.updated_data_leaves
				<< "\t" << res.pushed_leaves
				<< "\t" << res.intersection_approx_errors
				<< "\t" << res.intersection_approx_skips
				<< "\t" << res.elapsedTime.count() / 1000.0
				<< "\t" << res.meanTimePerLeaf().count()
		;
		
		return os;
	}
};


class Stock : public Model3D {
	
public:
	typedef boost::shared_ptr< Stock > Ptr;
	typedef boost::shared_ptr< const Stock > ConstPtr;
	
	typedef typename Octree< VoxelInfo >::DataView DataView;
	typedef Mesher< DataView > MesherType;
	
private:
	typedef Octree< VoxelInfo > _Octree;
	typedef Octree< VoxelInfo >::DataPtr VoxelInfoPtr;
	typedef Octree< VoxelInfo >::DataConstPtr VoxelInfoConstPtr;
	
private:
	const u_int MAX_DEPTH;
	const Eigen::Vector3d EXTENT;
	const Eigen::Translation3d STOCK_MODEL_TRASLATION;
	const VoxelInfoConstPtr DEFAULT_DATA;
	_Octree MODEL;
	mutable MesherType::Ptr MESHER;
	
public:
	Stock(const StockDescription &desc, u_int maxDepth, MesherType::Ptr mesher);
	virtual ~Stock();
	
	/**
	 * 
	 * @param cutter
	 * @param rototrasl rototraslation of the cutter in terms of this
	 * stock basis
	 * @return
	 */
	IntersectionResult intersect(const Cutter::ConstPtr &cutter, const Eigen::Isometry3d &rototrasl);
	
	Eigen::Vector3d getResolution() const;
	
	virtual Mesh::Ptr getMeshing();
	
private:
	
	/**
	 * 
	 * @param leaf
	 * @param cutter
	 * @param rototras cutter (not the bounding box) rototraslation in terms of model basis
	 * @return
	 */
	VoxelInfoPtr buildInfos(const _Octree::LeafConstPtr &leaf, 
			const Cutter::ConstPtr &cutter, const Eigen::Isometry3d &isometry);
	
	/**
	 * Calculate new waste produced by milling of given \c leaf according to
	 * erosion information contained in \c newInfo. \c newInfo will be updated
	 * inside the method in order to reflect all erosion happened so far to
	 * given \c leaf.
	 * 
	 * @param leaf
	 * @param newInfo
	 * @return
	 */
	double calculateWaste(const _Octree::LeafConstPtr &leaf,
			const VoxelInfoPtr &newInfo) const;
	
	/**
	 * 
	 * @param voxel
	 * @param oldInfo
	 * @param updatedInfo that is old VoxelInfo after they have been updated
	 * with new ones
	 * @return
	 */
	double getApproxWaste(const SimpleBox &box, const VoxelInfoConstPtr &oldInfo, const VoxelInfoConstPtr &updatedInfo) const;
	
	double intersectedVolume(const SimpleBox &box, const VoxelInfoConstPtr &info) const;
	
	bool canPushLevel(const _Octree::LeafPtr &leaf) const;
	
	friend std::ostream & operator<<(std::ostream &os, const Stock &stock);
};


#endif /* STOCK_HPP_ */
