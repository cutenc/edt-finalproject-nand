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
#include "common/AtomicNumber.hpp"
#include "configuration/StockDescription.hpp"
#include "meshing/Mesher.hpp"
#include "Cutter.hpp"
#include "VoxelInfo.hpp"
#include "Octree.hpp"
#include "SimpleBox.hpp"
#include "IntersectionResult.hpp"
#include "StoredData.hpp"


class Stock : public Model3D {
	
public:
	typedef boost::shared_ptr< Stock > Ptr;
	typedef boost::shared_ptr< const Stock > ConstPtr;
	
	typedef Octree OctreeType;
	typedef Mesher< StoredData > MesherType;
	
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
	
	typedef Octree::VersionInfo VersionInfo;
	typedef LeafNode::Ptr LeafPtr;
	typedef void (Stock::* Processer)(OctreeNode::Ptr,
			const CutterInfos &,
			const VersionInfo &,
			IntersectionResult &);
	
	typedef boost::lock_guard< boost::mutex > LockGuard;
	
	typedef AtomicNumber<u_int> Versioner;
	
private:
	const u_int MAX_DEPTH;
	const Eigen::Vector3d EXTENT;
	const Eigen::Translation3d STOCK_MODEL_TRASLATION;
	OctreeType MODEL;
	Processer PROCESSERS[2];
	StoredData::DeletedDataPtr deletedData;
	MesherType::Ptr MESHER;
	
	mutable boost::mutex mutex;
	u_int lastRetrievedVersion;
	
	Versioner versioner;
	
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
	
	const Eigen::Vector3d &getExtents() const;
	
	const Eigen::Translation3d &getStockModelTranslation() const;
	
	virtual Mesh::Ptr getMeshing();
	
private:
	
	void analyzeLeaf(OctreeNode::Ptr currLeaf, 
			const CutterInfos &cutterInfo, const VersionInfo &vinfo,
			IntersectionResult &results);
	
	void processTreeRecursive(OctreeNode::Ptr branch,
			const CutterInfos &cutInfo, const VersionInfo &vinfo,
			IntersectionResult &result);
	
	bool isNodeIntersecting(OctreeNode::ConstPtr node,
			const CutterInfos &cutInfo, bool accurate) const;
	
	void buildChangedNodesQueue(BranchNode::ConstPtr node,
			const VersionInfo &vinfo, StoredData::VoxelData &queue) const;
	
	struct WasteInfo {
		u_char newInsideCorners;
		u_char currentIntersectingCorners;
		
		void reset() {
			newInsideCorners = currentIntersectingCorners = 0;
		}
	};
	void cutVoxel(const LeafPtr &leaf, const CutterInfos &cutterInfo, WasteInfo &wasteInfo) const;
	
	double calculateNewWaste(const LeafPtr &currLeaf, const WasteInfo &info);
	
	bool canPushLevel(const LeafPtr &leaf) const;
	
	friend std::ostream & operator<<(std::ostream &os, const Stock &stock);
};


#endif /* STOCK_HPP_ */
