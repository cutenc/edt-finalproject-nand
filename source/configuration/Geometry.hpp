/*
 * GeometricDescriptor.hpp
 *
 *  Created on: 14/ago/2012
 *      Author: socket
 */

#ifndef GEOMETRY_HPP_
#define GEOMETRY_HPP_

#include <stdexcept>
#include <string>

#include <boost/shared_ptr.hpp>

class Geometry {
public:
	
	enum GeometryType {
		CYLINDER,
		SPHERE,
		RECTANGULAR_CUBOID,
		MESH
	};
	
	virtual ~Geometry() { }
	
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

class Cylinder : public Geometry {
public:
	Cylinder(const float r, const float h) : RADIUS(r), HEIGHT(h) { }
	virtual ~Cylinder() { }
	
	virtual GeometryType getType() const { return CYLINDER; }
	
	const float RADIUS, HEIGHT;
};

class Sphere : public Geometry {
public:
	Sphere(const float r) : RADIUS(r) { }
	virtual ~Sphere() { }
	
	virtual GeometryType getType() const { return SPHERE; }
	
	const float RADIUS;
};

class RectCuboid : public Geometry {
public:
	RectCuboid(const float x, const float y, const float z) :
		X(x), Y(y), Z(z) { }
	virtual ~RectCuboid() { }
	
	virtual GeometryType getType() const { return RECTANGULAR_CUBOID; }
	
	const float X, Y, Z;
};

class MeshGeometry : public Geometry {
public:
	MeshGeometry() { }
	virtual ~MeshGeometry() { }
	
	virtual GeometryType getType() const { return MESH; }
};


typedef boost::shared_ptr<Geometry> GeometryPtr;
typedef boost::shared_ptr<RectCuboid> RectCuboidPtr;


#endif /* GEOMETRY_HPP_ */
