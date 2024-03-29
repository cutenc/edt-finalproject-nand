/*
 * ShiftedBox.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef SHIFTEDBOX_HPP_
#define SHIFTEDBOX_HPP_

#include <ostream>
#include <cmath>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <Eigen/Geometry>

#include "Voxel.hpp"

class ShiftedBox {
	
public:
	
	typedef boost::shared_ptr< ShiftedBox > Ptr;
	typedef boost::shared_ptr< const ShiftedBox > ConstPtr;
	typedef Eigen::Translation<long double, 3> Translation3ld;
	typedef Eigen::Matrix<long double, 3, 1> Vector3ld;
	
//	typedef Eigen::Matrix<double, 3, 2> MinMaxMatrix;
//	typedef boost::shared_ptr< const MinMaxMatrix > MinMaxMPtr;
	
private:
	const SimpleBox::ConstPtr simpleBox;
	
	/* we decide to keep translation as a long double because the getShifted()
	 * method will be called "recursively" behaving like a long sum of terms.
	 * Such an operation can led to lose a lot of precision and one
	 * method used to mitigate this effect is doubling the precision of the
	 * sum, as described here:
	 * http://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
	 * (an alternative would have been Kahan summation formula but it offers
	 * worse error reduction at about the same impact in class behavior) 
	 */
	const Translation3ld shift;
	
//	const MinMaxMPtr minMax;
	
	
public:
	ShiftedBox(const SimpleBox::ConstPtr &box, const Eigen::Translation3d &tras) :
			simpleBox(box), shift(tras.cast< long double >())
			
//			,minMax(buildMinMax(box, tras))
	{ }
	
	explicit
	ShiftedBox(const SimpleBox::ConstPtr &box, const Translation3ld &tras) :
			simpleBox(box), shift(tras)
	
//			,minMax(buildMinMax(box, tras.cast< double >()))
	{ }
	
	virtual ~ShiftedBox() { }
	
	/**
	 * 
	 * @param tras
	 * @return another box shifted from current one according to given
	 * translation
	 */
	ShiftedBox getShifted(const Eigen::Translation3d &tras) const {
		Vector3ld tmpTrans = this->shift.translation() + 
				tras.translation().cast< long double >();
		
		Translation3ld newTrans(tmpTrans);
		
		return ShiftedBox(this->simpleBox, newTrans);
	}
	
	ShiftedBox getResized(const SimpleBox::ConstPtr &newSize) const {
		return ShiftedBox(newSize, this->shift);
	}
	
	Eigen::Vector3d getCorner(const Corner::CornerType &corner, const Eigen::Isometry3d &rototras) const {
		return (rototras * shift.cast< double >()) * simpleBox->getCorner(corner);
	}
	
	Eigen::Translation3d getShift() const {
		return this->shift.cast< double >();
	}
	
	const SimpleBox::ConstPtr & getSimpleBox() const {
		return this->simpleBox;
	}
	
	/**
	 * 
	 * @param otherBox
	 * @param rototras
	 * @param accurate tells wether check should be very accurate or not:
	 * if this flag is set to false some NON colliding cases will be threated
	 * as collinding ones but method execution will be ~3/7 times faster.
	 * @return
	 */
	bool isIntersecting(const SimpleBox &otherBox,
			const Eigen::Isometry3d &rototras,
			bool accurate) const {
		
		// thanks to http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php
				
		/*ALGORITHM: Use the separating axis test for all 15 potential
		separating axes. If a separating axis could not be found, the two
		boxes overlap. */
		
		double ra, rb, t;
		const Eigen::Vector3d &a = this->simpleBox->getHalfExtent(),
				&b = otherBox.getHalfExtent();
		
		const Eigen::Matrix3d &rotation = rototras.linear();
		const Eigen::Vector3d traslation = rototras.translation() - 
				this->shift.cast< double >().translation();
		
		//A's basis vectors
		for(int i = 0; i < 3; i++ ) {
			ra = a[i];
			rb = b[0] * fabs((double)rotation(i, 0)) 
					+ b[1] * fabs((double)rotation(i, 1)) 
					+ b[2] * fabs((double)rotation(i, 2)) ;
			t = fabs((double) traslation[i]);
			if( t > ra + rb )
				return false;
		}

		//B's basis vectors
		for(int i = 0; i<3 ; i++ ) {
			ra = a[0] * fabs((double)rotation(0, i))
					+ a[1] * fabs((double)rotation(1, i))
					+ a[2] * fabs((double)rotation(2, i)); 
			rb = b[i];
			t = fabs((double)(
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
			ra = a[1]*fabs((double)rotation(2, 0)) + a[2]*fabs((double)rotation(1, 0));
			rb = b[1]*fabs((double)rotation(0, 2)) + b[2]*fabs((double)rotation(0, 1));
			t = fabs((double)(traslation[2]*rotation(1, 0) - traslation[1]*rotation(2, 0)));
			if( t > ra + rb )
				return false;
	
			//L = A0 x B1
			ra = a[1]*fabs((double)rotation(2, 1)) + a[2]*fabs((double)rotation(1, 1));
			rb = b[0]*fabs((double)rotation(0, 2)) + b[2]*fabs((double)rotation(0, 0));
			t = fabs((double)(traslation[2]*rotation(1, 1) - traslation[1]*rotation(2, 1)));
			if( t > ra + rb )
				return false;
	
			//L = A0 x B2
			ra = a[1]*fabs((double)rotation(2, 2)) + a[2]*fabs((double)rotation(1, 2));
			rb = b[0]*fabs((double)rotation(0, 1)) + b[1]*fabs((double)rotation(0, 0));
			t = fabs((double)(traslation[2]*rotation(1, 2) - traslation[1]*rotation(2, 2)));
			if( t > ra + rb )
				return false;
	
			//L = A1 x B0
			ra = a[0]*fabs((double)rotation(2, 0)) + a[2]*fabs((double)rotation(0, 0));
			rb = b[1]*fabs((double)rotation(1, 2)) + b[2]*fabs((double)rotation(1, 1));
			t = fabs((double)(traslation[0]*rotation(2, 0) - traslation[2]*rotation(0, 0)));
			if( t > ra + rb )
				return false;
	
			//L = A1 x B1
			ra = a[0]*fabs((double)rotation(2, 1)) + a[2]*fabs((double)rotation(0, 1));
			rb = b[0]*fabs((double)rotation(1, 2)) + b[2]*fabs((double)rotation(1, 0));
			t = fabs((double)(traslation[0]*rotation(2, 1) - traslation[2]*rotation(0, 1)));
			if( t > ra + rb )
				return false;
	
			//L = A1 x B2
			ra = a[0]*fabs((double)rotation(2, 2)) + a[2]*fabs((double)rotation(0, 2));
			rb = b[0]*fabs((double)rotation(1, 1)) + b[1]*fabs((double)rotation(1, 0));
			t = fabs((double)(traslation[0]*rotation(2, 2) - traslation[2]*rotation(0, 2)));
			if( t > ra + rb )
				return false;
	
			//L = A2 x B0
			ra = a[0]*fabs((double)rotation(1, 0)) + a[1]*fabs((double)rotation(0, 0));
			rb = b[1]*fabs((double)rotation(2, 2)) + b[2]*fabs((double)rotation(2, 1));
			t = fabs((double)(traslation[1]*rotation(0, 0) - traslation[0]*rotation(1, 0)));
			if( t > ra + rb )
				return false;
	
			//L = A2 x B1
			ra = a[0]*fabs((double)rotation(1, 1)) + a[1]*fabs((double)rotation(0, 1));
			rb = b[0] *fabs((double)rotation(2, 2)) + b[2]*fabs((double)rotation(2, 0));
			t = fabs((double)(traslation[1]*rotation(0, 1) - traslation[0]*rotation(1, 1)));
			if( t > ra + rb )
				return false;
	
			//L = A2 x B2
			ra = a[0]*fabs((double)rotation(1, 2)) + a[1]*fabs((double)rotation(0, 2));
			rb = b[0]*fabs((double)rotation(2, 1)) + b[1]*fabs((double)rotation(2, 0));
			t = fabs((double)(traslation[1]*rotation(0, 2) - traslation[0]*rotation(1, 2)));
			if( t > ra + rb )
				return false;
			
		}

		/*no separating axis found,
		the two boxes overlap */

		return true;
	}
	
//	typedef std::pair<Eigen::Vector3d, Eigen::Vector3d> MinMaxVector;
//	bool isIntersecting(const MinMaxVector &minMax) const {
//		
//		if (thisMax[0] < minMax.first[0] || thisMin[0] > minMax.second[0])
//			return false;
//		if (thisMax[1] < minMax.first[1] || thisMin[1] > minMax.second[1])
//			return false;
//		if (thisMax[2] < minMax.first[2] || thisMin[2] > minMax.second[2])
//			return false;
//		return true;
//		
//	}
	
	
	friend std::ostream & operator<<(std::ostream &os, const ShiftedBox &sbox) {
		os << "SBOX[" << *sbox.simpleBox << "@(" 
				<< sbox.shift.translation().transpose() << ")]";
		
		return os;
	}
	
private:
	
//	static Eigen::Vector3d getCorner(const Corner::CornerType &c,
//			const MinMaxMatrix &minMax) {
//		
//		// with this matrix i choose wether to use MIN or MAX columns
//		static int minMaxLookupCol[][3] = {
//			/*	 X  Y  Z	*/
//		/*BFL*/	{0, 0, 0},
//		/*BFR*/	{1, 0, 0},
//		/*BRR*/	{1, 1, 0},
//		/*BRL*/	{0, 1, 0},
//		/*UFL*/	{0, 0, 1},
//		/*UFR*/	{1, 0, 1},
//		/*URR*/	{1, 1, 1},
//		/*URL*/	{0, 1, 1}
//		};
//		
//		int cIdx = static_cast< int >(c);
//		return Eigen::Vector3d(
//			minMax(0, minMaxLookupCol[cIdx][0]),
//			minMax(1, minMaxLookupCol[cIdx][1]),
//			minMax(2, minMaxLookupCol[cIdx][2])
//		);
//	}
	
//	inline
//	static MinMaxMPtr buildMinMax(const SimpleBox::ConstPtr &box, const Eigen::Translation3d &tras) {
//		
//		boost::shared_ptr< MinMaxMatrix > m = boost::allocate_shared< MinMaxMatrix, Eigen::aligned_allocator < MinMaxMatrix > >(
//					Eigen::aligned_allocator < MinMaxMatrix >()
//				);
//		
//		m->col(0) = box->getCorner(Corner::BottomFrontLeft) + tras.translation();
//		m->col(1) = box->getCorner(Corner::UpperRearRight) + tras.translation();
//		
//		return m;
//	}
	
};

#endif /* SHIFTEDBOX_HPP_ */
