#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "../PagShaderProgram.h"
#include "SceneObj.h"
#include "NodoScene.h"

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
	std::vector<NodoScene*> objetosScena; //Nota: Para cuando carge de texto
	NodoScene* nodo;
};

