/**
 * @file GeometricDescriptor.hpp
 *
 *  Created on: 14/ago/2012
 *      Author: socket
 *
 *  Contains the general Geometry class, plus all the available geometries an object can assume
 */

#ifndef GEOMETRY_HPP_
#define GEOMETRY_HPP_

#include <stdexcept>
#include <string>

#include <boost/shared_ptr.hpp>

#include <Eigen/Geometry>

#include <osg/Geometry>

/**
 * @class Geometry
 *
 * defines the geometry of an object.
 */
class Geometry {
public:
	
	/**
	 * the available shapes an object can assume
	 */
	enum GeometryType {
		CYLINDER,          //!< CYLINDER
		SPHERE,            //!< SPHERE
		RECTANGULAR_CUBOID,//!< RECTANGULAR_CUBOID
		MESH               //!< MESH
	};
	
	/**
	 * destructor
	 */
	virtual ~Geometry() { }
	
	/**
	 *
	 * @return the type of the object
	 */
	virtual GeometryType getType() const =0;
	
	/**
	 * Handy method used to convert \c this instance to a derived class object:
	 * use with caution
	 * @return
	 */
	template<typename T> const T& getAs() throw(std::runtime_error) {
		const T& tmp = dynamic_cast<const T &>(*this);
		
		return tmp;
	}

};

/**
 * @class Cylinder
 */
class Cylinder : public Geometry {
public:
	/**
	 * constructor
	 * @param r radius of the base
	 * @param h height
	 */
	Cylinder(const double r, const double h) : RADIUS(r), HEIGHT(h) { }
	virtual ~Cylinder() { }
	
	/**
	 *
	 * @return the type (cylinder)
	 */
	virtual GeometryType getType() const { return CYLINDER; }
	
	const double RADIUS, HEIGHT;
};

/**
 * @ class Sphere
 */
class Sphere : public Geometry {
public:
	/**
	 * constructor
	 * @param r the radius
	 */
	Sphere(const double r) : RADIUS(r) { }
	virtual ~Sphere() { }
	
	/**
	 *
	 * @return the type (sphere)
	 */
	virtual GeometryType getType() const { return SPHERE; }
	
	const double RADIUS;
};

/**
 * @class RectCuboid
 */
class RectCuboid : public Geometry {
public:
	/**
	 * constructor
	 * @param x length
	 * @param y width
	 * @param z height
	 */
	RectCuboid(const double x, const double y, const double z) :
		X(x), Y(y), Z(z) { }
	virtual ~RectCuboid() { }
	
	/**
	 *
	 * @return the type (cuboid)
	 */
	virtual GeometryType getType() const { return RECTANGULAR_CUBOID; }
	
	/**
	 *
	 * @return the cuboid as OSG vector of 3 coords
	 */
	osg::Vec3d asOsg() const {
		return osg::Vec3d(X, Y, Z);
	}
	
	/**
	 *
	 * @return the cuboid as Eigen vector of 3 coords
	 */
	Eigen::Vector3d asEigen() const {
		return Eigen::Vector3d(X, Y, Z);
	}
	
	/**
	 * the three coords
	 */
	const double X, Y, Z;
};

/**
 * @class MeshGeometry
 *
 * most general geometry
 */
class MeshGeometry : public Geometry {
public:
	/**
	 * constructor
	 */
	MeshGeometry() { }
	virtual ~MeshGeometry() { }
	
	/**
	 *
	 * @return the type (mesh)
	 */
	virtual GeometryType getType() const { return MESH; }
};


typedef boost::shared_ptr<Geometry> GeometryPtr;
typedef boost::shared_ptr<RectCuboid> RectCuboidPtr;


#endif /* GEOMETRY_HPP_ */
