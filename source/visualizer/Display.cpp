/*
 * Display.cpp
 *
 *  Created on: 27/set/2012
 *      Author: alberto
 */

#include "Display.hpp"

Display::Display() {
	// TODO Auto-generated constructor stub

}

Display::~Display() {
	// TODO Auto-generated destructor stub
}

//crea la scena e lancia il viewer
void draw(boost::shared_ptr<StockMesh> mesh) {

	InputDeviceStateType* idst = new InputDeviceStateType;

	std::cout << "input device state type created" << std::endl;

	osg::ref_ptr<osg::Group> root = osg::ref_ptr<osg::Group>(new osg::Group);
	osg::ref_ptr<osg::Box> unitCube = osg::ref_ptr<osg::Box>(new osg::Box( osg::Vec3(-2,-2,-2), 0.5f));
	osg::ref_ptr<osg::ShapeDrawable> unitCubeDrawable = osg::ref_ptr<osg::ShapeDrawable>(new osg::ShapeDrawable(unitCube));
	osg::ref_ptr<osg::Geode> basicShapesGeode = osg::ref_ptr<osg::Geode>(new osg::Geode);
//	basicShapesGeode->setDataVariance(osg::Object::DYNAMIC);
	basicShapesGeode->addDrawable(unitCubeDrawable);

	osg::ref_ptr<KeyboardManager> kh = osg::ref_ptr<KeyboardManager>(new KeyboardManager(idst));
	std::cout << "keyboard handler created" << std::endl;

	//basicShapesGeode->addUpdateCallback(sceneUpdater(basicShapesGeode));

	osg::ref_ptr<osg::Group> sonOfRoot = osg::ref_ptr<osg::Group>(new osg::Group);
	sonOfRoot->addChild(basicShapesGeode);
//	sonOfRoot->addChild(basicShapesGeode2);
	sonOfRoot->setDataVariance(osg::Object::DYNAMIC);
	sonOfRoot->setUpdateCallback(new SceneUpdater(idst));
//	root->addChild(sonOfRoot); // portato sotto, messo dentro la lightSource

	std::cout << "provo a creare il viewer... ";

	osg::ref_ptr<osgViewer::Viewer> viewer = osg::ref_ptr<osgViewer::Viewer>(new osgViewer::Viewer);

	std::cout << "fatto" << std::endl;

	viewer->setCameraManipulator(new osgGA::TrackballManipulator);
	viewer->setUpViewInWindow(100,100,800,600,0);
	viewer->addEventHandler(kh);

	osg::ref_ptr<osg::Light> light = osg::ref_ptr<osg::Light>(new osg::Light);
	// each light must have a unique number
	light->setLightNum(1);
	osg::ref_ptr<osg::LightSource> ls = osg::ref_ptr<osg::LightSource>(new osg::LightSource);
	ls->setLight(light);
	osg::StateSet *stateSet = root->getOrCreateStateSet();
	ls->setStateSetModes(*stateSet, osg::StateAttribute::ON);

	light->setAmbient(osg::Vec4f(1.0, 1.0, 1.0, 1.0));
	light->setDiffuse(osg::Vec4f(1.0, 1.0, 1.0, 1.0));
	light->setSpecular(osg::Vec4f(1.0, 1.0, 1.0, 1.0));
	light->setQuadraticAttenuation(0.95);

	light->setPosition(osg::Vec4f(0.0, 0.0, 0.0, 1.0));
	ls->setLocalStateSetModes(osg::StateAttribute::ON);
//	osg::StateSet *stateSet = new osg::StateSet();
//	root->setStateSet(stateSet);
//	ls->setStateSetModes(*stateSet, osg::StateAttribute::ON);

	ls->addChild(sonOfRoot);
	root->addChild(ls);

	root->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);

	osg::Geode* axis = new osg::Geode();
	axis->addDrawable(createAxis(osg::Vec3(0.0f,0.0f,0.0f),osg::Vec3(5.0f,0.0f,0.0f),osg::Vec3(0.0f,5.0f,0.0f),osg::Vec3(0.0f,0.0f,5.0f)));;
	root->addChild(axis);

	viewer->setSceneData( root );

	/*viewer->getCamera()-> setProjectionMatrixAsPerspective(40., 1., 1., 100. );

	osg::Matrix translateMatrix;
	translateMatrix.makeTranslate( 0., 0., -12. );*/

	viewer->realize();

	std::cout << "viewer run" << std::endl;

	viewer->run();
}

// le due funzioni qua sotto servono a creare gli assi
osg::Drawable* createSquare(const osg::Vec3& corner,const osg::Vec3& width,const osg::Vec3& height, osg::Image* image=NULL)
{
    // set up the Geometry.
    osg::Geometry* geom = new osg::Geometry;

    osg::Vec3Array* coords = new osg::Vec3Array(4);
    (*coords)[0] = corner;
    (*coords)[1] = corner+width;
    (*coords)[2] = corner+width+height;
    (*coords)[3] = corner+height;


    geom->setVertexArray(coords);

    osg::Vec3Array* norms = new osg::Vec3Array(1);
    (*norms)[0] = width^height;
    (*norms)[0].normalize();

    geom->setNormalArray(norms);
    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

    osg::Vec2Array* tcoords = new osg::Vec2Array(4);
    (*tcoords)[0].set(0.0f,0.0f);
    (*tcoords)[1].set(1.0f,0.0f);
    (*tcoords)[2].set(1.0f,1.0f);
    (*tcoords)[3].set(0.0f,1.0f);
    geom->setTexCoordArray(0,tcoords);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

    if (image)
    {
        osg::StateSet* stateset = new osg::StateSet;
        osg::Texture2D* texture = new osg::Texture2D;
        texture->setImage(image);
        stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
        geom->setStateSet(stateset);
    }

    return geom;
}

osg::Drawable* createAxis(const osg::Vec3& corner,const osg::Vec3& xdir,const osg::Vec3& ydir,const osg::Vec3& zdir)
{
    // set up the Geometry.
    osg::Geometry* geom = new osg::Geometry;

    osg::Vec3Array* coords = new osg::Vec3Array(6);
    (*coords)[0] = corner;
    (*coords)[1] = corner+xdir;
    (*coords)[2] = corner;
    (*coords)[3] = corner+ydir;
    (*coords)[4] = corner;
    (*coords)[5] = corner+zdir;

    geom->setVertexArray(coords);

    osg::Vec4 x_color(0.0f,1.0f,1.0f,1.0f);
    osg::Vec4 y_color(0.0f,1.0f,1.0f,1.0f);
    osg::Vec4 z_color(1.0f,0.0f,0.0f,1.0f);

    osg::Vec4Array* color = new osg::Vec4Array(6);
    (*color)[0] = x_color;
    (*color)[1] = x_color;
    (*color)[2] = y_color;
    (*color)[3] = y_color;
    (*color)[4] = z_color;
    (*color)[5] = z_color;

    geom->setColorArray(color);
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,6));

    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(4.0f);
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    geom->setStateSet(stateset);

    return geom;
}
