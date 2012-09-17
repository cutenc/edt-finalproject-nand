/*
 * ShiftedBox.hpp
 *
 *  Created on: 07/set/2012
 *      Author: socket
 */

#ifndef SHIFTEDBOX_HPP_
#define SHIFTEDBOX_HPP_

#include <ostream>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <Eigen/Geometry>

#include "Voxel.hpp"

class ShiftedBox {
	
public:
	
	typedef boost::shared_ptr< ShiftedBox > Ptr;
	typedef boost::shared_ptr< const ShiftedBox > ConstPtr;
	
private:
	
	const SimpleBox::ConstPtr simpleBox;
	const Eigen::Translation3d shift;
	
public:
	ShiftedBox(const SimpleBox::ConstPtr &box,
			const Eigen::Translation3d &tras = Eigen::Translation3d::Identity()) :
			simpleBox(box), shift(tras)
	{ }
	
	virtual ~ShiftedBox() { }
	
	/**
	 * 
	 * @param tras
	 * @return another box shifted from current one according to given
	 * translation
	 */
	ShiftedBox getShifted(const Eigen::Translation3d &tras) const {
		Eigen::Translation3d newTrans(this->shift.translation() + tras.translation());
		return ShiftedBox(this->simpleBox, newTrans);
	}
	
	ShiftedBox getResized(const SimpleBox::ConstPtr &newSize) const {
		return ShiftedBox(newSize, this->shift);
	}
	

	Voxel::Ptr getVoxel(const Eigen::Isometry3d &rototras = Eigen::Isometry3d::Identity()) const {
		SimpleBox::CornerMatrixPtr corners = getCornerMatrix(rototras);
		return boost::make_shared< Voxel >(corners);
	}
	
	/**
	 * 
	 * @param rototras isometric transformation expressed in this ShiftedBox
	 * basis: pay attention that total isometry will be
	 * \code
	 * rototras * this->shift
	 * \endcode
	 * that is, it will convert corners from current basis, according to
	 * given isometry.
	 * @return
	 */
	SimpleBox::CornerMatrixPtr getCornerMatrix(const Eigen::Isometry3d &rototras = Eigen::Isometry3d::Identity()) const {
		/* total isometry equals given one plus this box translation
		 */
		Eigen::Isometry3d totIsometry; totIsometry = rototras * this->shift;
		
		return this->simpleBox->getCorners(totIsometry);
	}
	
	const Eigen::Translation3d & getShift() const {
		return this->shift;
	}
	
	const SimpleBox::ConstPtr & getSimpleBox() const {
		return this->simpleBox;
	}
	
	/**
	 * 
	 * @param otherBox
	 * @param traslation
	 * @param rotation
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
		
		const Eigen::Matrix3d &rotation = rototras.rotation();
		const Eigen::Vector3d traslation = rototras.translation() - 
				this->shift.translation();
		
		//A's basis vectors
		for(int i = 0; i < 3; i++ ) {
			ra = a[i];
			rb = b[0] * std::abs<double>((double)rotation(i, 0)) 
					+ b[1] * std::abs<double>((double)rotation(i, 1)) 
					+ b[2] * std::abs<double>((double)rotation(i, 2)) ;
			t = std::abs<double>((double) traslation[i]);
			if( t > ra + rb )
				return false;
		}

		//B's basis vectors
		for(int i = 0; i<3 ; i++ ) {
			ra = a[0] * std::abs<double>((double)rotation(0, i))
					+ a[1] * std::abs<double>((double)rotation(1, i))
					+ a[2] * std::abs<double>((double)rotation(2, i)); 
			rb = b[i];
			t = std::abs<double>((double)(
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
			ra = a[1]*std::abs<double>((double)rotation(2, 0)) + a[2]*std::abs<double>((double)rotation(1, 0));
			rb = b[1]*std::abs<double>((double)rotation(0, 2)) + b[2]*std::abs<double>((double)rotation(0, 1));
			t = std::abs<double>((double)(traslation[2]*rotation(1, 0) - traslation[1]*rotation(2, 0)));
			if( t > ra + rb )
				return false;
	
			//L = A0 x B1
			ra = a[1]*std::abs<double>((double)rotation(2, 1)) + a[2]*std::abs<double>((double)rotation(1, 1));
			rb = b[0]*std::abs<double>((double)rotation(0, 2)) + b[2]*std::abs<double>((double)rotation(0, 0));
			t = std::abs<double>((double)(traslation[2]*rotation(1, 1) - traslation[1]*rotation(2, 1)));
			if( t > ra + rb )
				return false;
	
			//L = A0 x B2
			ra = a[1]*std::abs<double>((double)rotation(2, 2)) + a[2]*std::abs<double>((double)rotation(1, 2));
			rb = b[0]*std::abs<double>((double)rotation(0, 1)) + b[1]*std::abs<double>((double)rotation(0, 0));
			t = std::abs<double>((double)(traslation[2]*rotation(1, 2) - traslation[1]*rotation(2, 2)));
			if( t > ra + rb )
				return false;
	
			//L = A1 x B0
			ra = a[0]*std::abs<double>((double)rotation(2, 0)) + a[2]*std::abs<double>((double)rotation(0, 0));
			rb = b[1]*std::abs<double>((double)rotation(1, 2)) + b[2]*std::abs<double>((double)rotation(1, 1));
			t = std::abs<double>((double)(traslation[0]*rotation(2, 0) - traslation[2]*rotation(0, 0)));
			if( t > ra + rb )
				return false;
	
			//L = A1 x B1
			ra = a[0]*std::abs<double>((double)rotation(2, 1)) + a[2]*std::abs<double>((double)rotation(0, 1));
			rb = b[0]*std::abs<double>((double)rotation(1, 2)) + b[2]*std::abs<double>((double)rotation(1, 0));
			t = std::abs<double>((double)(traslation[0]*rotation(2, 1) - traslation[2]*rotation(0, 1)));
			if( t > ra + rb )
				return false;
	
			//L = A1 x B2
			ra = a[0]*std::abs<double>((double)rotation(2, 2)) + a[2]*std::abs<double>((double)rotation(0, 2));
			rb = b[0]*std::abs<double>((double)rotation(1, 1)) + b[1]*std::abs<double>((double)rotation(1, 0));
			t = std::abs<double>((double)(traslation[0]*rotation(2, 2) - traslation[2]*rotation(0, 2)));
			if( t > ra + rb )
				return false;
	
			//L = A2 x B0
			ra = a[0]*std::abs<double>((double)rotation(1, 0)) + a[1]*std::abs<double>((double)rotation(0, 0));
			rb = b[1]*std::abs<double>((double)rotation(2, 2)) + b[2]*std::abs<double>((double)rotation(2, 1));
			t = std::abs<double>((double)(traslation[1]*rotation(0, 0) - traslation[0]*rotation(1, 0)));
			if( t > ra + rb )
				return false;
	
			//L = A2 x B1
			ra = a[0]*std::abs<double>((double)rotation(1, 1)) + a[1]*std::abs<double>((double)rotation(0, 1));
			rb = b[0] *std::abs<double>((double)rotation(2, 2)) + b[2]*std::abs<double>((double)rotation(2, 0));
			t = std::abs<double>((double)(traslation[1]*rotation(0, 1) - traslation[0]*rotation(1, 1)));
			if( t > ra + rb )
				return false;
	
			//L = A2 x B2
			ra = a[0]*std::abs<double>((double)rotation(1, 2)) + a[1]*std::abs<double>((double)rotation(0, 2));
			rb = b[0]*std::abs<double>((double)rotation(2, 1)) + b[1]*std::abs<double>((double)rotation(2, 0));
			t = std::abs<double>((double)(traslation[1]*rotation(0, 2) - traslation[0]*rotation(1, 2)));
			if( t > ra + rb )
				return false;
			
		}

		/*no separating axis found,
		the two boxes overlap */

		return true;
	}
	
	friend std::ostream & operator<<(std::ostream &os, const ShiftedBox &sbox) {
		os << "SBOX[" << *sbox.simpleBox << "@(" 
				<< sbox.shift.translation().transpose() << ")]";
		
		return os;
	}
	
};

#endif /* SHIFTEDBOX_HPP_ */
