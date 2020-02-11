
#include "Cube.h"
#include "../Loaders/AssimpLoader.h"

Cube::Cube(float size)
{
	AssimpLoader* loader = new AssimpLoader();

	this->obj = loader->loadModelAssimpObj( Application::getInstance()->getPathApp() + "OpenGLEngineTFG\\BasicElement\\cube.obj");
	this->obj->Scale(size, size, size);

	delete loader;
}

Cube::~Cube()
{
	delete obj;
}

SceneObj* Cube::getSceneObj()
{
	return this->obj;
}