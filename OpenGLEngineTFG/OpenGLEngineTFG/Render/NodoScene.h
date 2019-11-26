#pragma once

#include <glm.hpp>
#include <vector>
#include "Model.h"
#include "SceneObj.h"

class NodoScene : public Model
{
public:
	NodoScene();
	~NodoScene();

	void addNodo(NodoScene* nodo);
	void addObj(SceneObj* obj);

	void InitObjs();
	void UpdateObjs(float deltaTime);
	void DrawObjs(glm::mat4 mViewProjection);

private:
	std::vector<NodoScene*> nodos;
	std::vector<SceneObj*> objs;

	/* Private functions */
	void InitObjsRecursive(NodoScene* nodo);
	void UpdateObjsRecursive(NodoScene* nodo, float deltaTime);
	void DrawObjsRecursive(NodoScene* nodo, glm::mat4& modelMatrix, glm::mat4 mViewProjection);
};
