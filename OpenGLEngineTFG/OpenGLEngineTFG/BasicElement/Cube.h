#pragma once

#include <glm.hpp>
#include <vector>
#include <string>

#include "../Render/SceneObj.h"

class Cube
{
public:
	Cube(float size);
	~Cube();

	SceneObj* getSceneObj();

private:
	//Datos del objeto
	std::vector<glm::vec3> puntos;
	std::vector<glm::vec3> normales;
	std::vector<glm::vec2> coord_textura;
	std::vector<GLuint> indices;

	//Obj
	SceneObj* obj;

	// Funciones
	std::string ExePath();
};

