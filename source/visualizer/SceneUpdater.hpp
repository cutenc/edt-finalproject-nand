/*
 * SceneUpdater.hpp
 *
 *  Created on: 10/ott/2012
 *      Author: alberto
 */

#ifndef SCENEUPDATER_HPP_
#define SCENEUPDATER_HPP_

#include "common/OSGMesh.hpp"
#include "threading/CyclicRunnable.hpp"
#include "visualizer/MillingSignaler.hpp"
#include "visualizer/InputDeviceStateType.hpp"


class SceneUpdater : public osg::NodeCallback {
public:
	SceneUpdater(InputDeviceStateType* ids) {
		idst = ids;
		i = 0.0f;
	}

//	callback per l'aggiornamento della scena
	virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv ) {
		osg::ref_ptr<osg::Group> group = node->asGroup(); //recupera l'oggetto cui fa riferimento la callback

		if(group) {
			if(idst->step) {
				try {
					SignaledInfo infos = signaler->awaitMiller();

//					Mesh::Ptr mesh = stock->getMeshing();
//					std::cout << "mesh: " << *mesh << std::endl;
//
//					std::cout << infos.lastMove << std::endl;

//					in attesa del marching cubes, questa roba qua sotto dovrebbe prendere i cubi
//					e inserirli nella scena al loro posto

					SignaledInfo::MillingData::const_iterator it;
					for (it = infos.millingResults->begin(); it != infos.millingResults->end(); ++it) {
						std::cout << *it << std::endl;

//						crea il cubo
						osg::ref_ptr<osg::Geometry> cube = osg::ref_ptr<osg::Geometry>(new osg::Geometry);
						cube = this->createCube();
						osg::ref_ptr<osg::Geode> geode = osg::ref_ptr<osg::Geode>(new osg::Geode);
						geode->addDrawable(cube);

//						crea la trasformazione, assegna i valori determinati dall'ultima mossa
						osg::ref_ptr<osg::MatrixTransform> transform = osg::ref_ptr<osg::MatrixTransform>(new osg::MatrixTransform);
						osg::Vec3f angle = osg::Vec3f(infos.lastMove.STOCK.ROTATION.ALPHA,
													  infos.lastMove.STOCK.ROTATION.BETA,
													  infos.lastMove.STOCK.ROTATION.GAMMA);
						osg::Vec3f axis = osg::Vec3f(infos.lastMove.STOCK.TRASLATION.getX(),
											         infos.lastMove.STOCK.TRASLATION.getY(),
											         infos.lastMove.STOCK.TRASLATION.getZ());
						transform->setMatrix(osg::Matrix::rotate(angle, axis));

//						inserisce il cubo come figlio della rotazione (lo ruota) e lo inserisce nella scena
						transform->addChild(geode);
						transform->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
						group->addChild(transform);
					}
				} catch (const MillerEndedException &e) {
					finished = true;
				}

//				controlla se deve andare avanti o attendere
				std::cout << "i = " << i << std::endl;
				i+=1;
				if(!idst->stepForever) {
					idst->step = false;
				} else {
					sleep(1);
				}
			}
		} else {
			std::cout << "fatal error: no ghe a fao a catar el geode" << std::endl;
			exit(1);
		}

		traverse(node, nv);
	}

	virtual ~SceneUpdater();

	protected:
		InputDeviceStateType* idst;
		float i;
		bool finished;
		const MillingSignaler::Ptr signaler;
		Stock::Ptr stock;

		static osg::ref_ptr<osg::Geometry> createCube() {
			osg::ref_ptr<osg::Geometry> cubeGeometry = osg::ref_ptr<osg::Geometry>(new osg::Geometry);

			// Specify the vertices:
			osg::ref_ptr<osg::Vec3Array> cubeVertices = osg::ref_ptr<osg::Vec3Array>(new osg::Vec3Array);
			cubeVertices->push_back( osg::Vec3(0, 0, 0) );
			cubeVertices->push_back( osg::Vec3(0, 0, 1) );
			cubeVertices->push_back( osg::Vec3(0, 1, 0) );
			cubeVertices->push_back( osg::Vec3(0, 1, 1) );
			cubeVertices->push_back( osg::Vec3(1, 0, 0) );
			cubeVertices->push_back( osg::Vec3(1, 0, 1) );
			cubeVertices->push_back( osg::Vec3(1, 1, 0) );
			cubeVertices->push_back( osg::Vec3(1, 1, 1) );


			// Associate this set of vertices with the geometry associated with the
			// geode we added to the scene.
			cubeGeometry->setVertexArray( cubeVertices );

			osg::ref_ptr<osg::DrawElementsUInt> cubeFace1 =
				osg::ref_ptr<osg::DrawElementsUInt>(new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4));
			cubeFace1->push_back(2);
			cubeFace1->push_back(3);
			cubeFace1->push_back(1);
			cubeFace1->push_back(0);
			cubeGeometry->addPrimitiveSet(cubeFace1);
	//		cubeGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS));

			osg::ref_ptr<osg::DrawElementsUInt> cubeFace2 =
				osg::ref_ptr<osg::DrawElementsUInt>(new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4));
			cubeFace2->push_back(1);
			cubeFace2->push_back(5);
			cubeFace2->push_back(4);
			cubeFace2->push_back(0);
			cubeGeometry->addPrimitiveSet(cubeFace2);
	//		cubeGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS));

		//
			osg::ref_ptr<osg::DrawElementsUInt> cubeFace3 =
				osg::ref_ptr<osg::DrawElementsUInt>(new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4));
			cubeFace3->push_back(4);
			cubeFace3->push_back(6);
			cubeFace3->push_back(2);
			cubeFace3->push_back(0);
			cubeGeometry->addPrimitiveSet(cubeFace3);
	//		cubeGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS));

		//
			osg::ref_ptr<osg::DrawElementsUInt> cubeFace4 =
				osg::ref_ptr<osg::DrawElementsUInt>(new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4));
			cubeFace4->push_back(5);
			cubeFace4->push_back(4);
			cubeFace4->push_back(6);
			cubeFace4->push_back(7);
			cubeGeometry->addPrimitiveSet(cubeFace4);
	//		cubeGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS));

			osg::ref_ptr<osg::DrawElementsUInt> cubeFace5 =
				osg::ref_ptr<osg::DrawElementsUInt>(new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4));
			cubeFace5->push_back(3);
			cubeFace5->push_back(2);
			cubeFace5->push_back(6);
			cubeFace5->push_back(7);
			cubeGeometry->addPrimitiveSet(cubeFace5);
	//		cubeGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS));

			osg::ref_ptr<osg::DrawElementsUInt> cubeFace6 =
				osg::ref_ptr<osg::DrawElementsUInt>(new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 4));
			cubeFace6->push_back(5);
			cubeFace6->push_back(1);
			cubeFace6->push_back(3);
			cubeFace6->push_back(7);
			cubeGeometry->addPrimitiveSet(cubeFace6);
	//		cubeGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS));

			osg::ref_ptr<osg::Vec3Array> normalArray = osg::ref_ptr<osg::Vec3Array>(new osg::Vec3Array);
			normalArray->push_back(osg::Vec3(-1, 0, 0));
			normalArray->push_back(osg::Vec3(0, -1, 0));
			normalArray->push_back(osg::Vec3(0, 0, -1));
			normalArray->push_back(osg::Vec3(+1, 0, 0));
			normalArray->push_back(osg::Vec3(0, +1, 0));
			normalArray->push_back(osg::Vec3(0, 0, +1));
			cubeGeometry->setNormalArray(normalArray);

			return cubeGeometry;
	}
};

#endif /* SCENEUPDATER_HPP_ */
