/**
 * ShiftedBox.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef SHIFTEDBOX_HPP_
#define SHIFTEDBOX_HPP_

#include <ostream>
#include <cmath>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <Eigen/Geometry>

#include <osg/BoundingBox>

#include "Corner.hpp"

/**
 * @class ShiftedBox
 *
 * Class representing a volume of space shaped as a box (rectangular cuboid),
 * aligned with stock axis, and shifted from stock center.
 */
class ShiftedBox : boost::noncopyable {
	
public:
	
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	
	typedef boost::shared_ptr< ShiftedBox > Ptr;
	typedef boost::shared_ptr< const ShiftedBox > ConstPtr;
	
	typedef Eigen::Matrix<double, 3, 2> MinMaxMatrix;
	typedef boost::shared_ptr< MinMaxMatrix > MinMaxPtr;
	typedef boost::shared_ptr< const MinMaxMatrix > MinMaxCPtr;
	
	static const int MIN_IDX = 0;
	static const int MAX_IDX = 1;
	
private:
	
	MinMaxMatrix MIN_MAX;
	Eigen::Vector3d EXTENTS;
	double VOLUME;
	
public:
	/**
	 * constructor
	 * @param center
	 * @param extents
	 */
	ShiftedBox(const Eigen::Vector3d &center, const Eigen::Vector3d &extents) :
		EXTENTS(extents)
	{
		assert(EXTENTS(0) > 0 && EXTENTS(1) > 0 && EXTENTS(2) > 0);
		
		calculateMinMax(MIN_MAX, center, extents);
		
		VOLUME = EXTENTS.prod();
		assert(VOLUME > 0);
	}
	
	/**
	 * constructor
	 * @param minMax
	 */
	ShiftedBox(const MinMaxMatrix &minMax) :
		MIN_MAX(minMax)
	{
		calculateExtentsAndVolume();
	}
	
	/**
	 * empty constructor
	 */
	ShiftedBox() { }
	
	/**
	 * destructor
	 */
	virtual ~ShiftedBox() { }
	
	/**
	 *
	 * @return volume of the box
	 */
	inline
	double getVolume() const {
		return this->VOLUME;
	}
	
	/**
	 *
	 * @return maximum elements of the box
	 */
	inline
	MinMaxMatrix::ConstColXpr getMax() const {
		return MIN_MAX.col(1);
	}
	
	/**
	 *
	 * @return minimum elements of the box
	 */
	inline
	MinMaxMatrix::ConstColXpr getMin() const {
		return MIN_MAX.col(0);
	}
	
	/**
	 *
	 * @return box matrix
	 */
	inline
	const MinMaxMatrix &getMatrix() const {
		return MIN_MAX;
	}
	
	/**
	 *
	 * @return box matrix
	 */
	inline
	MinMaxMatrix &getMatrix() {
		return MIN_MAX;
	}
	
	/**
	 * compute extents and volume of the box
	 */
	inline
	void calculateExtentsAndVolume() {
		EXTENTS.noalias() = MIN_MAX.col(MAX_IDX) - MIN_MAX.col(MIN_IDX);
		assert(EXTENTS(0) > 0 && EXTENTS(1) > 0 && EXTENTS(2) > 0);
		
		VOLUME = EXTENTS.prod();
		assert(VOLUME > 0);
	}
	
	/**
	 *
	 * @return extents of the box
	 */
	inline
	const Eigen::Vector3d &getExtents() const {
		return this->EXTENTS;
	}
	
	/**
	 *
	 * @param corner
	 * @param rototras
	 * @return desidered corner, rototraslated according to the given rototraslation
	 */
	inline
	Eigen::Vector3d getCorner(const Corner::CornerType &corner, const Eigen::Isometry3d &rototras) const {
		return rototras * getCorner(corner);
	}
	
	/**
	 *
	 * @param corner
	 * @return desidered corner
	 */
	inline
	Eigen::Vector3d getCorner(const Corner::CornerType &corner) const {
		return getCornerImp(corner, MIN_MAX);
	}
	
	/**
	 *
	 * @return vector of means of each row
	 */
	inline
	Eigen::Vector3d getShift() const {
		return MIN_MAX.rowwise().mean();
	}
	
	/**
	 *
	 * @return shifted box as bounding box.
	 */
	inline
	osg::BoundingBoxd asBoundingBox() const {
		/* specify every coord in order to avoid some useless vector
		 * creation & destruction
		 */ 
		return osg::BoundingBoxd(
				MIN_MAX(0, 0), MIN_MAX(1, 0), MIN_MAX(2, 0),
				MIN_MAX(0, 1), MIN_MAX(1, 1), MIN_MAX(2, 1)
		);
	}
	
	/**
	 *
	 * @param minMax
	 * @return True if given matrix intersects the box
	 */
	bool isIntersecting(const MinMaxMatrix &minMax) const {
		const MinMaxMatrix &thisMM = getMatrix();
		
		for (int i = 0; i < 3; ++i) {
			
			if(thisMM(i, MAX_IDX) < minMax(i, MIN_IDX)
				||
				thisMM(i, MIN_IDX) > minMax(i, MAX_IDX)) {
				
				return false;
			}
			
		}
		
		return true;
	}
	
	/**
	 *	Use the separating axis test for all 15 potential
	 *	separating axes. If a separating axis could not be found, the two
	 *	boxes overlap
	 * 
	 * @param accurate tells wether check should be accurate or not:
	 * if this flag is set to false some NON colliding cases will be threated
	 * as collinding ones but method execution will be ~3/7 times faster.
	 *
	 * @return True if given vector intersects the box
	 */
	bool isIntersecting(const Eigen::Vector3d &extents,
			const Eigen::Isometry3d &rototras,
			bool accurate) const {
		
		/* thanks to 
		 * http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php
		 */
				
		/*ALGORITHM: Use the separating axis test for all 15 potential
		separating axes. If a separating axis could not be found, the two
		boxes overlap. */
		
		double ra, rb, t;
		const Eigen::Vector3d a = this->getExtents() * 0.5,
				b = extents * 0.5;
		
		const Eigen::Matrix3d &rotation = rototras.linear();
		const Eigen::Vector3d traslation = rototras.translation() - 
				this->getShift();
		
		//A's basis vectors
		for(int i = 0; i < 3; i++ ) {
			ra = a[i];
			rb = b[0] * fabs(rotation(i, 0)) 
					+ b[1] * fabs(rotation(i, 1)) 
					+ b[2] * fabs(rotation(i, 2)) ;
			t = fabs(traslation[i]);
			if( t > ra + rb )
				return false;
		}

		//B's basis vectors
		for(int i = 0; i<3 ; i++ ) {
			ra = a[0] * fabs(rotation(0, i))
					+ a[1] * fabs(rotation(1, i))
					+ a[2] * fabs(rotation(2, i)); 
			rb = b[i];
			t = fabs((
					traslation[0] * rotation(0, i)
					+ traslation[1] * rotation(1, i)
					+ traslation[2] * rotation(2, i)
			));
			if( t > ra + rb )
				return false;
		}
		
		if (accurate) {
			//9 cross products
	
			//L = A0 x B0
			ra = a[1]*fabs(rotation(2, 0)) + a[2]*fabs(rotation(1, 0));
			rb = b[1]*fabs(rotation(0, 2)) + b[2]*fabs(rotation(0, 1));
			t = fabs((traslation[2]*rotation(1, 0) - traslation[1]*rotation(2, 0)));
			if( t > ra + rb )
				return false;
	
			//L = A0 x B1
			ra = a[1]*fabs(rotation(2, 1)) + a[2]*fabs(rotation(1, 1));
			rb = b[0]*fabs(rotation(0, 2)) + b[2]*fabs(rotation(0, 0));
			t = fabs((traslation[2]*rotation(1, 1) - traslation[1]*rotation(2, 1)));
			if( t > ra + rb )
				return false;
	
			//L = A0 x B2
			ra = a[1]*fabs(rotation(2, 2)) + a[2]*fabs(rotation(1, 2));
			rb = b[0]*fabs(rotation(0, 1)) + b[1]*fabs(rotation(0, 0));
			t = fabs((traslation[2]*rotation(1, 2) - traslation[1]*rotation(2, 2)));
			if( t > ra + rb )
				return false;
	
			//L = A1 x B0
			ra = a[0]*fabs(rotation(2, 0)) + a[2]*fabs(rotation(0, 0));
			rb = b[1]*fabs(rotation(1, 2)) + b[2]*fabs(rotation(1, 1));
			t = fabs(traslation[0]*rotation(2, 0) - traslation[2]*rotation(0, 0));
			if( t > ra + rb )
				return false;
	
			//L = A1 x B1
			ra = a[0]*fabs(rotation(2, 1)) + a[2]*fabs(rotation(0, 1));
			rb = b[0]*fabs(rotation(1, 2)) + b[2]*fabs(rotation(1, 0));
			t = fabs(traslation[0]*rotation(2, 1) - traslation[2]*rotation(0, 1));
			if( t > ra + rb )
				return false;
	
			//L = A1 x B2
			ra = a[0]*fabs(rotation(2, 2)) + a[2]*fabs(rotation(0, 2));
			rb = b[0]*fabs(rotation(1, 1)) + b[1]*fabs(rotation(1, 0));
			t = fabs(traslation[0]*rotation(2, 2) - traslation[2]*rotation(0, 2));
			if( t > ra + rb )
				return false;
	
			//L = A2 x B0
			ra = a[0]*fabs(rotation(1, 0)) + a[1]*fabs(rotation(0, 0));
			rb = b[1]*fabs(rotation(2, 2)) + b[2]*fabs(rotation(2, 1));
			t = fabs(traslation[1]*rotation(0, 0) - traslation[0]*rotation(1, 0));
			if( t > ra + rb )
				return false;
	
			//L = A2 x B1
			ra = a[0]*fabs(rotation(1, 1)) + a[1]*fabs(rotation(0, 1));
			rb = b[0] *fabs(rotation(2, 2)) + b[2]*fabs(rotation(2, 0));
			t = fabs(traslation[1]*rotation(0, 1) - traslation[0]*rotation(1, 1));
			if( t > ra + rb )
				return false;
	
			//L = A2 x B2
			ra = a[0]*fabs(rotation(1, 2)) + a[1]*fabs(rotation(0, 2));
			rb = b[0]*fabs(rotation(2, 1)) + b[1]*fabs(rotation(2, 0));
			t = fabs(traslation[1]*rotation(0, 2) - traslation[0]*rotation(1, 2));
			if( t > ra + rb )
				return false;
		}

		/*no separating axis found,
		the two boxes overlap */

		return true;
	}
	
//	friend std::ostream & operator<<(std::ostream &os, const ShiftedBox &sbox) {
//		os << "SBOX[" << *sbox.simpleBox << "@(" 
//				<< sbox.shift.translation().transpose() << ")]";
//		
//		return os;
//	}
	
	/**
	 * calculates MinMax matrix
	 * @param minMax
	 * @param isom
	 * @param extents
	 */
	static void calculateMinMax(MinMaxMatrix &minMax, const Eigen::Isometry3d &isom, const Eigen::Vector3d &extents) {
		// first build a origin-centerd minMax
		MinMaxMatrix tmp;
		tmp.col(0).noalias() = extents * -0.5;
		tmp.col(1).noalias() = extents * 0.5;
		
		// then apply the isometry to each corner finding new min-max
		assert(Corner::N_CORNERS >= 1);
		minMax.col(MIN_IDX).noalias() = minMax.col(MAX_IDX).noalias() = isom * getCornerImp(0, tmp);
		
		for (int i = 1; i < Corner::N_CORNERS; ++i) {
			Eigen::Vector3d newCorner = isom * getCornerImp(i, tmp);
			for (int c = 0; c < 3; ++c) {
				if (newCorner(c) < minMax(c, MIN_IDX)) {
					// less than min
					minMax(c, MIN_IDX) = newCorner(c);
				} else if (newCorner(c) > minMax(c, MAX_IDX)) {
					// more than max
					minMax(c, MAX_IDX) = newCorner(c);
				}
			}
		}
	}
	
	/**
	 * calculates MinMax matrix
	 * @param minMax
	 * @param center
	 * @param extents
	 */
	static void calculateMinMax(MinMaxMatrix &minMax, const Eigen::Vector3d &center, const Eigen::Vector3d &extents) {
		minMax.col(0).noalias() = center - extents * 0.5;
		minMax.col(1).noalias() = center + extents * 0.5;
	}
	
private:
	
	static Eigen::Vector3d getCornerImp(int c, const MinMaxMatrix &minMax) {
		
		// with this matrix i choose wether to use MIN or MAX columns
		static const int minMaxLookupCol[][3] = {
			/*	 X  Y  Z	*/
		/*BFL*/	{0, 0, 0},
		/*BFR*/	{1, 0, 0},
		/*BRR*/	{1, 1, 0},
		/*BRL*/	{0, 1, 0},
		/*UFL*/	{0, 0, 1},
		/*UFR*/	{1, 0, 1},
		/*URR*/	{1, 1, 1},
		/*URL*/	{0, 1, 1}
		};
		
		assert(c < 8); // (Corner::N_CORNERS == 8) MUST hold
		
		return Eigen::Vector3d(
			minMax(0, minMaxLookupCol[c][0]),
			minMax(1, minMaxLookupCol[c][1]),
			minMax(2, minMaxLookupCol[c][2])
		);
	}
	
};

#endif /* SHIFTEDBOX_HPP_ */
