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
	float waste;
	
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
	
	boost::chrono::microseconds elapsedTime;
	
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
				<< "\t#elapsed time (ms)"
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

/**
 * Define a DataTraits specialization for the VoxelInfo class
 */
template<>
struct DataTraits< VoxelInfo > {
	typedef boost::shared_ptr< VoxelInfo > type;
	typedef boost::shared_ptr< const VoxelInfo > const_type;
	typedef const_type & reference;
	typedef const const_type & const_reference;
	
	static const_reference DEFAULT_DATA() {
		static const_type DATA = boost::make_shared< 
				const VoxelInfo >(VoxelInfo::DEFAULT_INSIDENESS());
		return DATA;
	}
};


class Stock : public Model3D {
	
public:
	typedef boost::shared_ptr< Stock > Ptr;
	typedef boost::shared_ptr< const Stock > ConstPtr;
	
	typedef Octree< VoxelInfo, DataTraits< VoxelInfo > > OctreeType;
	typedef typename OctreeType::DataView DataView;
	typedef Mesher< DataView > MesherType;
	
private:
	typedef DataTraits< VoxelInfo >::type VoxInfoPtr;
	typedef DataTraits< VoxelInfo >::const_type VoxInfoCPtr;
	typedef DataTraits< VoxelInfo >::reference VoxInfoCPtrRef;
	typedef DataTraits< VoxelInfo >::const_reference VoxInfoCPtrCRef;
	
private:
	const u_int MAX_DEPTH;
	const Eigen::Vector3f EXTENT;
	const Eigen::Translation3f STOCK_MODEL_TRASLATION;
	OctreeType MODEL;
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
	IntersectionResult intersect(const Cutter::ConstPtr &cutter, const Eigen::Isometry3f &rototrasl);
	
	Eigen::Vector3f getResolution() const;
	
	virtual Mesh::Ptr getMeshing();
	
private:
	
	/**
	 * 
	 * @param leaf
	 * @param cutter
	 * @param rototras cutter (not the bounding box) rototraslation in terms of model basis
	 * @return
	 */
	void buildInfos(const OctreeType::LeafConstPtr &leaf, 
			const Cutter::ConstPtr &cutter, const Eigen::Isometry3f &isometry,
			SimpleBox::CornerMatrix &cachedMatrix, VoxelInfo &info);
	
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
	float calculateWaste(const OctreeType::LeafConstPtr &leaf, VoxelInfo &newInfo) const;
	
	/**
	 * 
	 * @param voxel
	 * @param oldInfo
	 * @param updatedInfo that is old VoxelInfo after they have been updated
	 * with new ones
	 * @return
	 */
	float getApproxWaste(const SimpleBox &box, const VoxelInfo &oldInfo, const VoxelInfo &updatedInfo) const;
	
	float intersectedVolume(const SimpleBox &box, const VoxelInfo &info) const;
	
	bool canPushLevel(const OctreeType::LeafPtr &leaf) const;
	
	friend std::ostream & operator<<(std::ostream &os, const Stock &stock);
};


#endif /* STOCK_HPP_ */
