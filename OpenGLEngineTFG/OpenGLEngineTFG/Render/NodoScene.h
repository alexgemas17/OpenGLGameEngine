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

	void InitObjs();
	void UpdateObjs();
	void DrawObjs(PagShaderProgram* shader, glm::mat4 matrixVP);

private:
	std::vector<NodoScene*> nodos;
	std::vector<SceneObj*> objs;

	/* Private functions */
	void InitObjsRecursive(NodoScene* nodo);
	void UpdateObjsRecursive(NodoScene* nodo);
	void DrawObjsRecursive(PagShaderProgram* shader, NodoScene* nodo, glm::mat4& modelMatrix);
};
