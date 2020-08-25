#pragma once

#include <glm.hpp>
#include <vector>
#include "Model.h"
#include "SceneObj.h"
#include "../PagShaderProgram.h"
#include "../Camara/Camara.h"

class NodoScene : public Model
{
public:
	NodoScene();
	~NodoScene();

	void addNodo(NodoScene* nodo);
	void addObj(SceneObj* obj);

	void InitObjs();
	void UpdateObjs(float deltaTime);
	void setUniforms();
	void DrawObjs( PagShaderProgram* shader, const TypeDraw& type );

	SceneObj* getObj(int index) { return this->objs[index]; }
	std::vector<SceneObj*> getObjs() { return this->objs; }
	NodoScene* getNode(int index) { return this->nodos[index]; }
	std::vector<NodoScene*> getNodos() { return this->nodos; }

private:
	std::vector<NodoScene*> nodos;
	std::vector<SceneObj*> objs;

	/* Private functions */
	void InitObjsRecursive(NodoScene* nodo);

	void UpdateObjsRecursive(
		NodoScene* nodo,
		float deltaTime
	);

	void SetUniformsRecursive(
		NodoScene* nodo,
		glm::mat4& modelMatrix
	);
	
	void DrawObjsRecursive(
		PagShaderProgram* shader,
		NodoScene* nodo,
		const TypeDraw& type
	);
};
