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
#include "IntersectionResult.hpp"


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
