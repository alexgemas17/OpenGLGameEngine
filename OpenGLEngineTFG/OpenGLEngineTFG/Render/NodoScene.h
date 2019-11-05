#pragma once

#include <glm.hpp>
#include <vector>
#include "Model.h"
#include "SceneObj.h"

#include "../PagShaderProgram.h"

class NodoScene : public Model
{
public:
	NodoScene();
	~NodoScene();

	void addNodo(NodoScene* nodo);
	void addObj(SceneObj* obj);
	void DrawObjs(PagShaderProgram* shader);

private:
	std::vector<NodoScene*> nodos;
	std::vector<SceneObj*> objs;

	void DrawObjsRecursive(PagShaderProgram* shader, NodoScene* nodo, glm::mat4 modelMatrix);
};
