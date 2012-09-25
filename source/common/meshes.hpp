/*
 * meshes.hpp
 *
 *  Created on: 25/set/2012
 *      Author: socket
 */

#ifndef MESHES_HPP_
#define MESHES_HPP_

class StockMesh : public Mesh {
	
private:
	std::string msg;
	
public:
	StockMesh(std::string msg) : msg(msg) { }
	virtual ~StockMesh() { }
	
	virtual std::ostream & toOutStream(std::ostream &os) const {
		os << msg;
		
		return os;
	}
};


class StubMesh : public Mesh {
	
public:
	StubMesh() { }
	virtual ~StubMesh() { }
	
	virtual std::ostream & toOutStream(std::ostream &os) const {
		os << "HI, I'M STUB MESH!";
		
		return os;
	}
	
};

#endif /* MESHES_HPP_ */
