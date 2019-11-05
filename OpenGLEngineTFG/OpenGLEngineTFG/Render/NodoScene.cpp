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

void NodoScene::DrawObjs(PagShaderProgram* shader)
{
	// DATOS C�MARA BORRAR
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	// DATOS VENTANA -> �TENER C�MARA?
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1024.0f / 576.0f, 0.1f, 100.0f);

	glm::mat4 model = this->getModelMatrix();
	model = projection * view * model;
	DrawObjsRecursive(shader, this, model);
}

void NodoScene::InitObjs()
{
	InitObjsRecursive(this);
}

void NodoScene::UpdateObjs()
{
	UpdateObjsRecursive(this);
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

void NodoScene::UpdateObjsRecursive(NodoScene* nodo)
{
	//TODO
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