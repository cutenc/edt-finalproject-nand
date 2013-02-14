/**
 * @file MeshingVoxel.hpp
 *
 * Created on: 22/ott/2012
 * Author: alberto
 *
 * describes the voxel for Marching Cubes algorithm
 */

#ifndef MESHINGVOXEL_HPP_
#define MESHINGVOXEL_HPP_

#include <cassert>

#include <Eigen/Geometry>

#include "common/Utilities.hpp"
#include "milling/Corner.hpp"
#include "milling/ShiftedBox.hpp"
#include "milling/VoxelInfo.hpp"
#include "meshing/MeshingUtils.hpp"

/**
 * @class MeshingVoxel
 *
 * describes the voxel for Marching Cubes algorithm
 */
class MeshingVoxel {

private:
	
	/**
	 * This one will translate a Marching Cube corner index in a shifted box
	 * corner index
	 */
	static const Corner::CornerType CORNER_CONVERSION[8];
	
	/** the ShiftedBox object for the voxel */
	const ShiftedBox *sbox;
	
	/**
	 *  char => float conversion is automatic but this way i should not care
	 * about +/-0 float problem
	 */
	char weights[8];

public:
	/**
	 * Used classic pointers instead of smart ones just to speedup
	 * operations because otherwise two new shared pointer would have 
	 * been created and their counters increased each time an object of
	 * this class would have been created
	 *  
	 * @param sbox
	 * @param vinfo
	 */
	MeshingVoxel(const ShiftedBox *sbox, const VoxelInfo *vinfo, const Eigen::Vector3d &stockHalfExtents) :
		sbox(sbox)
	{
		/* this assert is due to the static '8' used to initialize
		 * CORNER_CONVERSION: i cannot write CORNER_CONVERSION[Corner::N_CORNERS]
		 * because of the "static initialization order fiasco" that would
		 * force me to modify Corner::N_CORNERS in something like
		 * Corner::N_CORNERS() breaking legacy code
		 * 
		 * http://www.parashift.com/c%2B%2B-faq-lite/static-init-order.html
		 */
		assert(Corner::N_CORNERS == 8);
		
		/* every corner weight is asked at least once and for some of them
		 * there will be a second ask, so build right now the weight array
		 */
		for (int i = 0; i < Corner::N_CORNERS; ++i) {
			Corner::CornerType c = CORNER_CONVERSION[i];
			char value;
			
			if (vinfo->isCornerCut(c)) {
				value = +1;
			} else {
				Eigen::Vector3d corner(sbox->getCorner(CORNER_CONVERSION[i]));
				value = (int)MeshingUtils::isBorderCorner(stockHalfExtents, corner) - 1;
			}
			
			weights[i] = value;
		}
		
	}
	
	/**
	 * returns desired corner in an OSG fashion
	 *
	 * @param i : selects the (i+1)-th corner
	 * @return the OSG coords of the corner
	 */
	inline
	osg::Vec3 getCornerOsg(int i) const {
		assert(i < Corner::N_CORNERS);
		
		return GeometryUtils::toOsg(sbox->getCorner(CORNER_CONVERSION[i]));
	}
	
	/**
	 * returns desired corner in an Eigen fashion
	 *
	 * @param i : selects the (i+1)-th corner
	 * @return the Eigen coords of the corner
	 */
	inline
	Eigen::Vector3d getCornerEigen(int i) const {
		assert(i < Corner::N_CORNERS);
		return sbox->getCorner(CORNER_CONVERSION[i]);
	}
	
	/**
	 * Values higher than 0 means points outside surface, values
	 * lower than 0 means points inside surface, values equals to 0 means
	 * points on the surface
	 * @param i
	 * @return
	 */
	inline
	float getWeight(int i) const {
		return weights[i];
	}
	
};

#endif /* MESHINGVOXEL_HPP_ */
