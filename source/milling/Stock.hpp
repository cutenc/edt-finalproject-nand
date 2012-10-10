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
	 * one partial erosions (that is it has been deleted before beeing found
	 * totally inside the cutter).
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
	
	boost::chrono::nanoseconds meanTimePerLeaf() const {
		if (this->analyzed_leaves == 0)
			return boost::chrono::nanoseconds(0);
		
		boost::chrono::nanoseconds us = elapsedTime;
		return boost::chrono::nanoseconds(us.count() / this->analyzed_leaves);
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
				<< "\t" << res.meanTimePerLeaf().count() / 1000.0
		;
		
		return os;
	}
	
	IntersectionResult & operator+=(const IntersectionResult &other) {
		waste += other.waste;
		analyzed_leaves += other.analyzed_leaves;
		purged_leaves += other.purged_leaves;
		lazy_purged_leaves += other.lazy_purged_leaves;
		pushed_leaves += other.pushed_leaves;
		updated_data_leaves += other.updated_data_leaves;
		intersection_approx_errors += other.intersection_approx_errors; 
		intersection_approx_skips += other.intersection_approx_skips;
		elapsedTime += other.elapsedTime;
		
		return *this;
	}
	
	const IntersectionResult operator+(const IntersectionResult &other) const {
		return IntersectionResult(*this) += other;
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
	
	friend std::ostream & operator<<(std::ostream &os, const_reference vinfo) {
		return os << *vinfo;
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
	const Eigen::Vector3d EXTENT;
	const Eigen::Translation3d STOCK_MODEL_TRASLATION;
	const u_int MAX_THREADS;
	OctreeType MODEL;
	mutable MesherType::Ptr MESHER;
	
public:
	Stock(const StockDescription &desc, u_int maxDepth, u_int maxThreads, MesherType::Ptr mesher);
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
	
	struct CutterInfos {
		typedef boost::shared_ptr< CutterInfos > Ptr;
		typedef boost::shared_ptr< const CutterInfos > ConstPtr;
		
		const Cutter::ConstPtr cutter;
		const SimpleBox bbox;
		const boost::shared_ptr< Eigen::Isometry3d > cutterIsom_model, bboxIsom_model;
		
		CutterInfos(const Cutter::ConstPtr &cutter,
				const Eigen::Vector3d &bboxExtents,
				const Eigen::Isometry3d &cutterIsom_model,
				const Eigen::Isometry3d &bboxIsom_model) :
					cutter(cutter), bbox(bboxExtents),
					cutterIsom_model(boost::allocate_shared< Eigen::Isometry3d, Eigen::aligned_allocator< Eigen::Isometry3d > >(Eigen::aligned_allocator< Eigen::Isometry3d >(), cutterIsom_model)),
					bboxIsom_model(boost::allocate_shared< Eigen::Isometry3d, Eigen::aligned_allocator< Eigen::Isometry3d > >(Eigen::aligned_allocator< Eigen::Isometry3d >(), bboxIsom_model))
		{
		}
		
		virtual ~CutterInfos() { }
	};
	
	OctreeType::OperationType analyzeLeaf(const OctreeType::LeafPtr &currLeaf, 
			const CutterInfos &cutterInfo, IntersectionResult &results);
	
	
	struct WasteInfo {
		u_char newInsideCorners;
		u_char totInserted;
		
		void reset() {
			newInsideCorners = totInserted = 0;
		}
	};
	void cutVoxel(const OctreeType::LeafConstPtr &leaf, const CutterInfos &cutterInfo,
			VoxelInfo &voxelInfo, WasteInfo &wasteInfo) const;
	
	double calculateNewWaste(const OctreeType::LeafPtr &currLeaf, const WasteInfo &info);
	
	bool canPushLevel(const OctreeType::LeafPtr &leaf) const;
	
	friend std::ostream & operator<<(std::ostream &os, const Stock &stock);
};


#endif /* STOCK_HPP_ */
