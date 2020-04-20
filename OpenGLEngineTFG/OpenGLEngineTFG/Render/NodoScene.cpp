#include "NodoScene.h"
#include <gtc/matrix_transform.hpp>

#include "../Application.h"

NodoScene::NodoScene(): Model(){}
NodoScene::~NodoScene() {}

void NodoScene::addNodo(NodoScene* nodo)
{
	this->nodos.push_back(nodo);
}

void NodoScene::addObj(SceneObj* obj)
{
	this->objs.push_back(obj);
}

void NodoScene::DrawObjs(PagShaderProgram* shader, const TypeDraw& type)
{
	glm::mat4 model = this->getModelMatrix();
	DrawObjsRecursive(shader, this, model, type);
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
	if (nodo) {
		if (nodo->nodos.empty()) {
			if (!nodo->objs.empty()) {
				for (int i = 0; i < nodo->objs.size(); i++) {
					nodo->objs[i]->Init();
					Application::getInstance()->getMainScene()->objs.push_back(nodo->objs[i]);
				}
			}
		}
		else {
			for (int i = 0; i < nodo->nodos.size(); i++) {
				InitObjsRecursive(nodo->nodos[i]);
			}

			if (!nodo->objs.empty()) {
				for (int i = 0; i < nodo->objs.size(); i++) {
					nodo->objs[i]->Init();
					Application::getInstance()->getMainScene()->objs.push_back(nodo->objs[i]);
				}
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

//Draw genérico donde se especifica el tipo de shader
void NodoScene::DrawObjsRecursive(PagShaderProgram* shader, NodoScene* nodo, glm::mat4& _modelMatrix, const TypeDraw& type)
{
	if (nodo->nodos.empty()) {
		if (!nodo->objs.empty()) {
			for (int i = 0; i < nodo->objs.size(); i++) {
				glm::mat4 model = _modelMatrix * nodo->objs[i]->getModelMatrix();
				nodo->objs[i]->DrawObj(shader, model, type);
			}
		}
	}
	else {
		for (int i = 0; i < nodo->nodos.size(); i++) {
			glm::mat4 model = _modelMatrix * nodo->nodos[i]->getModelMatrix();
			DrawObjsRecursive(shader, nodo->nodos[i], model, type);
		}

		if (!nodo->objs.empty()) {
			for (int i = 0; i < nodo->objs.size(); i++) {
				glm::mat4 model = _modelMatrix * nodo->objs[i]->getModelMatrix();
				nodo->objs[i]->DrawObj(shader, model, type);
			}
		}
	}
}