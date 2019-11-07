#include "NodoScene.h"
#include <gtc/matrix_transform.hpp>

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

void NodoScene::DrawObjs(PagShaderProgram* shader, glm::mat4 matrixVP)
{
	glm::mat4 model = this->getModelMatrix();
	model = matrixVP * model;
	DrawObjsRecursive(shader, this, model);
}

void NodoScene::InitObjs()
{
	InitObjsRecursive(this);
}

void NodoScene::UpdateObjs(float deltaTime)
{
	UpdateObjsRecursive(this, deltaTime);
}

/* --------------------- PRIVATE FUNCIONS ---------------------*/
void NodoScene::InitObjsRecursive(NodoScene* nodo)
{
	if (nodo->nodos.empty()) {
		if (!nodo->objs.empty()) {
			for (int i = 0; i < nodo->objs.size(); i++) {
				nodo->objs[i]->Init();
			}
		}
	}
	else {
		for (int i = 0; i < nodo->nodos.size(); i++) {
			InitObjsRecursive(this->nodos[i]);
		}

		if (!nodo->objs.empty()) {
			for (int i = 0; i < nodo->objs.size(); i++) {
				nodo->objs[i]->Init();
			}
		}
	}
}

void NodoScene::UpdateObjsRecursive(NodoScene* nodo, float deltaTime)
{
	if (nodo->nodos.empty()) {
		if (!nodo->objs.empty()) {
			for (int i = 0; i < nodo->objs.size(); i++) {
				nodo->objs[i]->UpdateObj(deltaTime);
			}
		}
	}
	else {
		for (int i = 0; i < nodo->nodos.size(); i++) {
			UpdateObjsRecursive(nodo, deltaTime);
		}

		if (!nodo->objs.empty()) {
			for (int i = 0; i < nodo->objs.size(); i++) {
				nodo->objs[i]->UpdateObj(deltaTime);
			}
		}
	}
}

void NodoScene::DrawObjsRecursive(PagShaderProgram* shader, NodoScene* nodo, glm::mat4 &modelMatrix)
{
	if (nodo->nodos.empty()) {
		if (!nodo->objs.empty()) {
			for (int i = 0; i < nodo->objs.size(); i++) {
				glm::mat4 model = modelMatrix * nodo->objs[i]->getModelMatrix();
				nodo->objs[i]->DrawObj(shader, model);
			}
		}
	}
	else {
		for (int i = 0; i < nodo->nodos.size(); i++) {
			glm::mat4 model = modelMatrix * nodo->nodos[i]->getModelMatrix();
			DrawObjsRecursive(shader, nodo->nodos[i], model);
		}

		if (!nodo->objs.empty()) {
			for (int i = 0; i < nodo->objs.size(); i++) {
				glm::mat4 model = modelMatrix * nodo->objs[i]->getModelMatrix();
				nodo->objs[i]->DrawObj(shader, model);
			}
		}
	}
}