#include "Cube.h"
#include "../Loaders/AssimpLoader.h"

#include "../Application.h"

Cube::Cube(float size)
{
	AssimpLoader* loader = new AssimpLoader();

	std::string path = Application::getInstance()->getPath();
	this->obj = loader->loadModelAssimpObj(path + "OpenGLEngineTFG\\BasicElement\\cube.obj", "");
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