/**
 * Face.hpp
 *
 *  Created on: 19/nov/2012
 *      Author: socket
 */

#ifndef FACE_HPP_
#define FACE_HPP_

#include "MeshingUtils.hpp"

/**
 * @class Face
 *
 * builds the face of a voxel
 */
class Face {
	
public:
	enum Type {
		LEFT = 0,
		FRONT,
		BOTTOM,
		RIGHT,
		REAR,
		TOP
	};
	
	static const int N_FACES = 6;
	
	static const Corner::CornerType FACE_ADJACENCY[6][4];
	
public:
	
	inline
	static bool isBorderFace(Face::Type face, const Eigen::Vector3d &halfExtents, const ShiftedBox &sBox) {
		for (int i = 0; i < 4; ++i) {
			if (!MeshingUtils::isBorderCorner(halfExtents, sBox.getCorner(FACE_ADJACENCY[face][i]))) {
				return false;
			}
		}
		
		return true;
	}
};

/**
 * @class FaceIterator
 *
 * builds all the needed faces
 */
class FaceIterator : public std::iterator<std::input_iterator_tag, Face::Type> {

private:
	static const Face::Type END_FLAG = static_cast<Face::Type>(-1);
	Face::Type curr;
	
public:
	/**
	 * constructor
	 * @param face
	 */
	FaceIterator(Face::Type face) : curr(face) { }
	FaceIterator() : curr(END_FLAG) { }
	
	virtual ~FaceIterator() { }
	
	/**
	 * overrides the ++ operator to go iterate through the faces
	 * @return
	 */
	FaceIterator& operator++() {
		assert(!isEnded());
		
		if (this->curr == Face::TOP) {
			this->curr = FaceIterator::END_FLAG;
		} else {
			int tmp = this->curr;
			this->curr = static_cast<Face::Type>(++tmp);
		}
		
		return *this;
	}
	
	/**
	 * overrides the == operator
	 * @param oi
	 * @return
	 */
	bool operator==(const FaceIterator &oi) {
		return this->curr == oi.curr;
	}
	
	/**
	 * overrides the != operator
	 * @param oi
	 * @return
	 */
	bool operator!=(const FaceIterator &oi) {
		return this->curr != oi.curr;
	}
	
	/**
	 * overrides the * operator
	 * @return
	 */
	Face::Type operator*() {
		assert(!isEnded());
		
		return this->curr;
	}
	
	/**
	 *
	 * @return first face
	 */
	static FaceIterator begin() { return FaceIterator(Face::LEFT); }

	/**
	 *
	 * @return last face
	 */
	static FaceIterator end() { return FaceIterator(); }
	
private:
	bool isEnded() {
		return this->curr == FaceIterator::END_FLAG;
	}
};


#endif /* FACE_HPP_ */
