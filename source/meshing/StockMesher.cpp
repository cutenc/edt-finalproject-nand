/*
 * StockMesher.cpp
 *
 *  Created on: 22/set/2012
 *      Author: socket
 */

#include "StockMesher.hpp"

#include <sstream>

StockMesher::StockMesher() {
}

StockMesher::~StockMesher() {
}

Mesh::Ptr StockMesher::buildMesh(const StoredData &data) {
	std::stringstream ss;
	ss << "# of data=" << data.getData()->size();
	
	
	
	
	
	
// 	// crea il cubo
//	osg::ref_ptr<osg::Geometry> cube = osg::ref_ptr<osg::Geometry>(new osg::Geometry);
//	cube = this->createCube();
//	osg::ref_ptr<osg::Geode> geode = osg::ref_ptr<osg::Geode>(new osg::Geode);
//	geode->addDrawable(cube);
//
//	// crea la trasformazione, assegna i valori determinati dall'ultima mossa
//	osg::ref_ptr<osg::MatrixTransform> transform = osg::ref_ptr<osg::MatrixTransform>(new osg::MatrixTransform);
//	osg::Vec3f angle = osg::Vec3f(infos.lastMove.STOCK.ROTATION.ALPHA,
//								  infos.lastMove.STOCK.ROTATION.BETA,
//								  infos.lastMove.STOCK.ROTATION.GAMMA);
//	osg::Vec3f axis = osg::Vec3f(infos.lastMove.STOCK.TRASLATION.getX(),
//								 infos.lastMove.STOCK.TRASLATION.getY(),
//								 infos.lastMove.STOCK.TRASLATION.getZ());
//	transform->setMatrix(osg::Matrix::rotate(angle, axis));
//
//	// inserisce il cubo come figlio della rotazione (lo ruota) e lo inserisce nella scena
//	transform->addChild(geode);
//	transform->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//	group->addChild(transform);
	
	
	
	
	
	
	
	
	
	throw std::runtime_error("NOT IMPLEMENTED YET");
}
