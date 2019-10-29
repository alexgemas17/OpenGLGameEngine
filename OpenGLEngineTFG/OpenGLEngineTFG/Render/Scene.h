#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "../PagShaderProgram.h"

//---------- INCLUDE PRUEBAS --------------
#include "Render.h"

class Scene
{
public:
	Scene();
	~Scene();

	void InitObjs();
	void UpdateObjs();
	void DrawObjs(PagShaderProgram* shader);

private:

	//---------- VARIABLES DE PRUEBA --------------
	std::vector<glm::vec3> verticesPrueba;
	Render* triangulo;
};
