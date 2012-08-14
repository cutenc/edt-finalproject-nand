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
	 * Handy method used to convert \t this instance to a derived class object:
	 * use with caution
	 * @return
	 */
	template<typename T> const T* getAs() throw(std::runtime_error) {
		const T* tmp = dynamic_cast<const T *>(this);
		if (!tmp)
			throw std::runtime_error(std::string("cannot cast to: const") + 
					typeid(T).name() + "*");
		return tmp;
	}

};

typedef boost::shared_ptr<Geometry> GeometryPtr;

class Cylinder : public Geometry {
public:
	Cylinder(double r, double h) : RADIUS(r), HEIGHT(h) { }
	virtual ~Cylinder() { }
	
	virtual GeometryType getType() const { return CYLINDER; }
	
	const double RADIUS, HEIGHT;
};

class Sphere : public Geometry {
public:
	Sphere(double r) : RADIUS(r) { }
	virtual ~Sphere() { }
	
	virtual GeometryType getType() const { return SPHERE; }
	
	const double RADIUS;
};

class RectCuboid : public Geometry {
public:
	RectCuboid(double l, double h, double w) :
		LENGTH(l), HEIGHT(h), WIDTH(w) { }
	virtual ~RectCuboid() { }
	
	virtual GeometryType getType() const { return RECTANGULAR_CUBOID; }
	
	const double LENGTH, HEIGHT, WIDTH;
};

class MeshGeometry : public Geometry {
public:
	MeshGeometry() { }
	virtual ~MeshGeometry() { }
	
	virtual GeometryType getType() const { return MESH; }
};

#endif /* GEOMETRY_HPP_ */
