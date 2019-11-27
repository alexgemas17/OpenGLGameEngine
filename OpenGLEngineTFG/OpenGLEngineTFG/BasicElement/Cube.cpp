#include "Cube.h"
#include "../Loaders/AssimpLoader.h"

#include <windows.h>
#include <iostream>

Cube::Cube(float size)
{
	AssimpLoader* loader = new AssimpLoader();

	std::string path = ExePath();
	this->obj = loader->loadModelAssimpObj(ExePath() + "OpenGLEngineTFG\\BasicElement\\cube.obj");
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

std::string Cube::ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");

	//pos - 5 --> Para quitar Debug de la ruta.
	return std::string(buffer).substr(0, pos - 5);
}
