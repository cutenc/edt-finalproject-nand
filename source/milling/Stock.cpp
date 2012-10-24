/*
 * Stock.cpp
 *
 *  Created on: 23/ago/2012
 *      Author: socket
 */

#include "Stock.hpp"

#include <vector>
#include <deque>
#include <stdexcept>
#include <cmath>
#include <cassert>

#include <boost/utility.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/assign/ptr_list_inserter.hpp>

#include <Eigen/Geometry>

#include <osg/PositionAttitudeTransform>

#include "SimpleBox.hpp"
#include "Corner.hpp"
#include "StoredData.hpp"

Stock::Stock(const StockDescription &desc, u_int maxDepth, MesherType::Ptr mesher) :
	MAX_DEPTH(maxDepth),
	EXTENT(desc.getGeometry()->X, desc.getGeometry()->Y, desc.getGeometry()->Z),
	STOCK_MODEL_TRASLATION(EXTENT / 2.0),
	MODEL(EXTENT, maxDepth),
	deletedData(boost::make_shared< StoredData::DeletedData >()),
	MESHER(mesher), lastRetrievedVersion(0), versioner(2)
{
	GeometryUtils::checkExtent(EXTENT);
	if(MAX_DEPTH <= 0)
		throw std::invalid_argument("max depth should be >0");
	
	PROCESSERS[0] = &Stock::processTreeRecursive;
	PROCESSERS[1] = &Stock::analyzeLeaf;
}

Stock::~Stock() { }

IntersectionResult Stock::intersect(const Cutter::ConstPtr &cutter,
		const Eigen::Isometry3d &rototras) {
	
	boost::chrono::thread_clock::time_point startTime = boost::chrono::thread_clock::now();
	
	Cutter::BoundingBoxInfo bboxInfo = cutter->getBoundingBox();
	
	/* in order to find bbox isometry i can think about converting a bbox point
	 * to a stock point: first the point should be converted bbox=>cutter basis
	 * with bboxInfo.rototraslation than another conversion is needed, that is,
	 * cutter=>stock basis using rototras. So the multiplication order should
	 * be the following (remember that the point is multiplied on the right):
	 * 
	 * Eigen::Isometry3d bboxIsom_stock = rototras * bboxInfo.rototraslation;
	 * 
	 * Now we have bboxIsom in respect to stock basis but we also need it
	 * in respect of MODEL basis: from stock to model there is only a
	 * translation (no rotation at all) and it is "additive" (in fact we have
	 * to perform a subtraction) to given rototras so...
	 * 
	 * Eigen::Isometry3d bboxIsom_model = STOCK_MODEL_TRASLATION.inverse() * bboxIsom_stock;
	 * 
	 * all summed up:
	 */
	
	Eigen::Isometry3d cutterIsom_model = STOCK_MODEL_TRASLATION.inverse() * rototras;
	
	Eigen::Isometry3d bboxIsom_model = cutterIsom_model * bboxInfo.rototraslation;
	
	CutterInfos::ConstPtr cutterInfo = boost::make_shared< const CutterInfos >(
			cutter, bboxInfo.extents, cutterIsom_model, bboxIsom_model);
	
		
//		static ShiftedBox::MinMaxVector getBoundingBoxMinMax(
//				const SimpleBox &obb,
//				const Eigen::Isometry3d &isom)
//				{
//			
//			SimpleBox::CornerMatrixConstPtr matrix = obb.getCorners(isom);
//			Eigen::Vector3d min, max;
//			
//			min = max = matrix->col(0);
//			
//			for (int c = 1; c < 8; ++c) {
//				for (int r = 0; r < 3; ++r) {
//					double v = (*matrix)(r, c);
//					if (v < min[r]) {
//						min[r] = v;
//					} else if (v > max[r]) {
//						max[r] = v;
//					}
//				}
//			}
//			return ShiftedBox::MinMaxVector(min, max);
//		}
//		
//		static bool isIntersecting(OctreeNode::ConstPtr node,
//				const ShiftedBox::MinMaxVector &boxMinMax) {
//			
//			const ShiftedBox::ConstPtr &currBox = node->getBox();
//			return currBox->isIntersecting(boxMinMax);
//		}

	IntersectionResult results;
	
	BranchNode::Ptr root = MODEL.getRoot();
	
	{
		LockGuard l(mutex);
		VersionInfo vinfo(lastRetrievedVersion, versioner.get() + 1);
		processTreeRecursive(root, *cutterInfo, vinfo, results);
		/* we completed the production of the new version so now we can 
		 * update versioner. It would have been wrong to update versioner
		 * during VersionInfo creation because the new version wouldn't have
		 * been completed yet (and theoretically should not be sent outside)
		 */
		versioner.incAndGet();
	}
	
	results.elapsedTime = boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::thread_clock::now() - startTime);
	
	return results;
}

void Stock::processTreeRecursive(OctreeNode::Ptr branchNode,
		const CutterInfos &cutInfo,
		const VersionInfo &vinfo,
		IntersectionResult &results) {
	
	BranchNode::Ptr branch = static_cast< BranchNode::Ptr >(branchNode);
	
	for (u_char i = 0; i < BranchNode::N_CHILDREN; ++i) {
		if (!branch->hasChild(i)) {
			continue;
		}
		
		OctreeNode::Ptr child = branch->getChild(i);
		if (!isNodeIntersecting(child, cutInfo, true)) {
			continue;
		}
		
		int procIdx = static_cast< int >(child->getType());
		
		assert(procIdx >= 0 && procIdx < 2);
		
		(this->*(PROCESSERS[procIdx]))(child, cutInfo, vinfo, results);
	}
	
	if (branch->isEmpty()) {
		MODEL.deleteBranch(branch);
	}
	
}

bool Stock::isNodeIntersecting(OctreeNode::ConstPtr node, const CutterInfos &cutInfo, bool accurate) const {
	return node->getBox()->isIntersecting(cutInfo.bbox, *cutInfo.bboxIsom_model, accurate);
}

void Stock::analyzeLeaf(OctreeNode::Ptr leaf, 
		const CutterInfos &cutterInfo,
		const VersionInfo &vinfo,
		IntersectionResult &results) {
	
	LeafPtr currLeaf = static_cast< LeafPtr >(leaf);
	
	assert(!currLeaf->getData()->isContained());
	
	results.analyzed_leaves++;
	
	/* by now we only know that cutter bounding box is intersecting
	 * currLeaf (if the test has set to be faster but inaccurate it may
	 * even not touching) but we have no clue about the fact that real
	 * cutter is intersecting or not.
	 * In order to do so we have to try to push model depth as deep as
	 * allowed in order to check if some voxels are fully contained or, at
	 * least, some of their corners are inside/outside cutter blade
	 */
	
	WasteInfo waste;
	cutVoxel(currLeaf, cutterInfo, waste);
	
	if (currLeaf->getData()->isContained()) {
		
		results.purged_leaves++;
		results.waste += calculateNewWaste(currLeaf, waste);
		
		// add stored info to the deleted data deque
		deletedData->push_back(currLeaf->getData());
		
		// then delete currLeaf from the model
		MODEL.deleteLeaf(currLeaf);
		
	} else {
		
		// currLeaf is probably half inside and half outside
		
		if (canPushLevel(currLeaf)) {
			
			results.pushed_leaves++;
			
			// pushing cause current leaf to be deleted
			deletedData->push_back(currLeaf->getData());
			
			// we can push another level so let's do it...
			BranchNode::Ptr newBranch = MODEL.pushLeaf(currLeaf, vinfo);
			
			// ... and recursively process it
			processTreeRecursive(newBranch, cutterInfo, vinfo, results);
			
		} else {
			
			/* leaf is intersecting but i cannot push more levels
			 * so let's update saved data incrementing waste count!
			 * 
			 * we reach this point even in the following conditions:
			 * cutter is really intersecting but we don't have enough voxel
			 * resolution or we stuck upon a bounding-box approximation error;
			 */
			
			results.updated_data_leaves++;
			
			double newWaste = calculateNewWaste(currLeaf, waste);
			results.waste += newWaste;
			
			if (newWaste > 0)
				MODEL.updateData(currLeaf, vinfo);
			
		} // if (canPushLevel)
	} // if (isContained)	
	
}


void Stock::cutVoxel(const LeafPtr &leaf,
		const CutterInfos &cutterInfo, WasteInfo &waste) const {
	
	/* we have to convert stockPoint in cutter basis: given isometry
	 * is for the cutter in respect of model basis, so we
	 * have to invert it to get model's roto-traslation in respect of
	 * cutter basis, that is, the isometry that converts model points in
	 * cutter points.
	 */
	const ShiftedBox::ConstPtr &box = leaf->getBox();
	const Eigen::Isometry3d modelIsom_cutter = cutterInfo.cutterIsom_model->inverse();
	
	VoxelInfo::Ptr info = leaf->getData();
	waste.reset();
	for (CornerIterator cit = CornerIterator::begin(); cit != CornerIterator::end(); ++cit) {
		if(info->isCornerCut(*cit)) {
			continue;
		}
		
		Eigen::Vector3d point = box->getCorner(*cit, modelIsom_cutter);
		
		double distance = cutterInfo.cutter->getDistance(point);
		
		bool newInside = info->updateInsideness(*cit, distance);
		
		if (newInside) {
			++waste.newInsideCorners;
		}
	}
}

double Stock::calculateNewWaste(const LeafPtr &currLeaf, const WasteInfo &info) {
	return currLeaf->getBox()->getSimpleBox()->getVolume() * 
			info.newInsideCorners / (double) Corner::N_CORNERS;
}


bool Stock::canPushLevel(const LeafPtr &leaf) const {
	return leaf->getDepth() < this->MAX_DEPTH;
}


Eigen::Vector3d Stock::getResolution() const {
	return (this->EXTENT / std::pow(2.0, (double)this->MAX_DEPTH));
}


const Eigen::Vector3d& Stock::getExtents() const {
	return this->EXTENT;
}

const Eigen::Translation3d& Stock::getStockModelTranslation() const {
	return this->STOCK_MODEL_TRASLATION;
}

std::ostream & operator<<(std::ostream &os, const Stock &stock) {
	os << "STOCK(extent=[" << stock.EXTENT.transpose()
			<< "];maxDepth=" << stock.MAX_DEPTH
			<< ";minBlockSize=[" << stock.getResolution().transpose()
			<< "])"
//			<< std::endl << stock.MODEL
			;
	
	return os;
}

void Stock::buildChangedNodesQueue(BranchNode::ConstPtr node,
			const VersionInfo &vinfo, StoredData::VoxelData &queue) const {

	for(int i = 0; i < BranchNode::N_CHILDREN; ++i) {
		if (!node->hasChild(i)) {
			continue;
		}
		
		OctreeNode::Ptr child = node->getChild(i);
		if(!child->isChanged(vinfo)) {
			continue;
		}
		
		switch (child->getType()) {
			case OctreeNode::BRANCH_NODE:
				buildChangedNodesQueue(
						static_cast< BranchNode::ConstPtr >(child),
						vinfo,
						queue
				);
				break;
			case OctreeNode::LEAF_NODE: {
				LeafNode::Ptr leaf = static_cast< LeafNode::Ptr >(child);
				StoredData::VoxelPair vpair(
						leaf->getBox(),
						leaf->getData()
				);
				queue.push_back(vpair);
				break;
			}
			default:
				throw std::runtime_error("Unknown node type");
				break;
		}
	}
}

Mesh::Ptr Stock::getMeshing() {
	const static osg::Vec3d TRANSLATION(
			STOCK_MODEL_TRASLATION.translation()[0],
			STOCK_MODEL_TRASLATION.translation()[1],
			STOCK_MODEL_TRASLATION.translation()[2]
	);
	
	StoredData::VoxelDataPtr data = boost::make_shared< StoredData::VoxelData >();
	StoredData::DeletedDataPtr newDeleted = boost::make_shared< StoredData::DeletedData >();
	BranchNode::ConstPtr root = MODEL.getRoot();
	{
		// acquire lock
		LockGuard l(mutex);
		
		// build & update version informations
		VersionInfo currVinfo(lastRetrievedVersion, versioner.get());
		lastRetrievedVersion = versioner.get();
		
		// build new & updated data queue
		if (root->isChanged(currVinfo)) {
			buildChangedNodesQueue(root, currVinfo, *data);
		}
		
		/* TODO nella gestione della concorrenza rimane un problema: i
		 * VoxelInfo mandati al mesher sono comunque condivisi con il miller
		 * quindi potrebbe accadere che per due voxel adiacenti il mesher ne
		 * calcoli uno con un vertice eroso e un'altro con il vertice ancora
		 * posizionato. In caso di stepping mode (o comunque nell'ultima
		 * visualizzazione) tutti questi artifatti spariscono in quanto
		 * milling e meshing hanno una relazione happens-before
		 */
		
		// renew deletedDataPtr
		deletedData.swap(newDeleted);
		
		// release lock
	}
	
	// build StoredData based on updatedData & deletedData
	StoredData storedData(data, newDeleted);
	
	// invoke mesher
	Mesh::Ptr mesh = MESHER->buildMesh(storedData);
	
	// apply translation to given mesh
	osg::PositionAttitudeTransform *PAT = new osg::PositionAttitudeTransform;
	PAT->setPosition(TRANSLATION);
	PAT->addChild(mesh->getMesh().get());
	
	return boost::make_shared< Mesh >(PAT);
}

