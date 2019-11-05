#include "NodoScene.h"

NodoScene::NodoScene(): Model() {}
NodoScene::~NodoScene() {}

void NodoScene::addNodo(NodoScene* nodo)
{
	this->nodos.push_back(nodo);
}

void NodoScene::addObj(SceneObj* obj)
{
	this->objs.push_back(obj);
}

void NodoScene::DrawObjs(PagShaderProgram* shader)
{
	for (int i = 0; i < this->nodos.size(); i++)
		DrawObjsRecursive(shader, this->nodos[i], this->getModelMatrix());
}

void NodoScene::DrawObjsRecursive(PagShaderProgram* shader, NodoScene* nodo, glm::mat4 modelMatrix)
{
	if (nodo->nodos.empty()) {
		if (!nodo->objs.empty())
			for (int i = 0; i < nodo->objs.size(); i++)
				nodo->objs[i]->DrawObj(shader, (modelMatrix * nodo->objs[i]->getModelMatrix()));
	}
	else {
		for (int i = 0; i < nodo->nodos.size(); i++)
			DrawObjsRecursive(shader, this->nodos[i], (modelMatrix * nodo->getModelMatrix()));
	}
}