/*
 * Stock.hpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#ifndef STOCK_HPP_
#define STOCK_HPP_

#include <cassert>
#include <ostream>

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
		const Cutter::ConstPtr cutter;
		const Eigen::Vector3d *extents;
		const Eigen::Isometry3d *cutterIsom_model;
		const Eigen::Isometry3d *bboxIsom_model;
		const ShiftedBox::MinMaxMatrix *minMax;
		
		CutterInfos(const Cutter::ConstPtr &cutter,
				const Eigen::Vector3d *bboxExtents,
				const Eigen::Isometry3d *cutterIsom_model,
				const Eigen::Isometry3d *bboxIsom_model,
				const ShiftedBox::MinMaxMatrix *minMax) :
					cutter(cutter), extents(bboxExtents),
					cutterIsom_model(cutterIsom_model),
					bboxIsom_model(bboxIsom_model),
					minMax(minMax)
		{
		}
		
		virtual ~CutterInfos() { }
	};
	
	class DeletedDataQueuer {
	private:
		typedef void (DeletedDataQueuer::* Queuer)(const VoxelInfo::Ptr &);
		
	private:
		StoredData::DeletedDataPtr deletedData;
		int queuerIdx;
		Queuer queuers[2];
		
	public:
		DeletedDataQueuer() :
			deletedData(boost::make_shared< StoredData::DeletedData >()),
			queuerIdx(0)
		{
			queuers[0] = &DeletedDataQueuer::stubQueuer;
			queuers[1] = &DeletedDataQueuer::realQueuer;
		}
		virtual ~DeletedDataQueuer() { }
		
		void activate() {
			queuerIdx = 1;
		}
		
		void enqueue(const VoxelInfo::Ptr &data) {
			assert(queuerIdx < 2);
			(this->*(queuers[queuerIdx]))(data);
		}
		
		StoredData::DeletedDataPtr renewQueue() {
			StoredData::DeletedDataPtr oldQueue = boost::make_shared< StoredData::DeletedData >();
			deletedData.swap(oldQueue);
			return oldQueue;
		}
		
	private:
		void stubQueuer(const VoxelInfo::Ptr &) {
			// do nothing
		}
		
		void realQueuer(const VoxelInfo::Ptr &data) {
			deletedData->push_back(data);
		}
	};
	
	class IntersectionTester {
		
	private:
		typedef bool (IntersectionTester::* TestFoo)(const ShiftedBox &, const CutterInfos &) const;
		
	private:
		static const int N_DIVISIONS = 2;
		const u_int depthSwitch;
		TestFoo TESTS[N_DIVISIONS];
		
	public:
		IntersectionTester(u_int maxDepth) :
			depthSwitch( fmin(4.0, maxDepth) )
		{
			int i = 0;
			TESTS[i++] = &IntersectionTester::accurateInt;
			TESTS[i++] = &IntersectionTester::fastInt;
			assert(i == N_DIVISIONS);
		}
		
		bool isIntersecting(const OctreeNode::Ptr &node, const CutterInfos &cutInfo) const {
			/* chose the intersection test based upon tree depth: use more
			 * accurate tests at higher levels and then switch to faster ones
			 * when depth increase (and the number of leaves to analyze explode)
			 */
			int idx = node->getDepth() >= depthSwitch;
			
			assert(idx < N_DIVISIONS);
			return (this->*(TESTS[idx]))(*node->getBox(), cutInfo);
		}
		
	private:
//		bool veryAccurateInt(const ShiftedBox &sbox, const CutterInfos &cutInfo) const {
//			return sbox.isIntersecting(*cutInfo.extents, *cutInfo.bboxIsom_model, true);
//		}
		
		bool accurateInt(const ShiftedBox &sbox, const CutterInfos &cutInfo) const {
			return sbox.isIntersecting(*cutInfo.extents, *cutInfo.bboxIsom_model, false);
		}
		
		bool fastInt(const ShiftedBox &sbox, const CutterInfos &cutInfo) const {
			return sbox.isIntersecting(*cutInfo.minMax);
		}
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
	const IntersectionTester intersectionTester;
	MesherType::Ptr MESHER;
	u_int lastRetrievedVersion;
	Versioner versioner;
	
	mutable boost::mutex mutex;
	Processer PROCESSERS[2];
	DeletedDataQueuer deletedQueuer;
	
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
	
	void buildChangedNodesQueue(BranchNode::ConstPtr node,
			const VersionInfo &vinfo, StoredData::VoxelData &queue) const;
	
	struct WasteInfo {
		int newInsideCorners;
		
		void reset() {
			newInsideCorners = 0;
		}
	};
	void cutVoxel(const LeafPtr &leaf, const CutterInfos &cutterInfo, WasteInfo &wasteInfo) const;
	
	double calculateNewWaste(const LeafPtr &currLeaf, const WasteInfo &info);
	
	bool canPushLevel(const LeafPtr &leaf) const;
	
	friend std::ostream & operator<<(std::ostream &os, const Stock &stock);
};


#endif /* STOCK_HPP_ */
