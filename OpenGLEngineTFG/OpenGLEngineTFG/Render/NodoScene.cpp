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
	DrawObjsRecursive(shader, this, type);
}

void NodoScene::InitObjs()
{
	InitObjsRecursive(this);
}

void NodoScene::UpdateObjs(float deltaTime)
{
	UpdateObjsRecursive(this, deltaTime);
}

void NodoScene::setUniforms()
{
	glm::mat4 model = this->getModelMatrix();
	SetUniformsRecursive(this, model);
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

//Set uniforms
void NodoScene::SetUniformsRecursive(NodoScene* nodo, glm::mat4& _modelMatrix)
{
	if (nodo->nodos.empty()) {
		if (!nodo->objs.empty()) {
			for (int i = 0; i < nodo->objs.size(); i++) {
				glm::mat4 model = _modelMatrix * nodo->objs[i]->getModelMatrix();
				nodo->objs[i]->SetUniforms(model);
			}
		}
	}
	else {
		for (int i = 0; i < nodo->nodos.size(); i++) {
			glm::mat4 model = _modelMatrix * nodo->nodos[i]->getModelMatrix();
			SetUniformsRecursive(nodo->nodos[i], model);
		}

		if (!nodo->objs.empty()) {
			for (int i = 0; i < nodo->objs.size(); i++) {
				glm::mat4 model = _modelMatrix * nodo->objs[i]->getModelMatrix();
				nodo->objs[i]->SetUniforms(model);
			}
		}
	}
}

//Draw genérico donde se especifica el tipo de shader
void NodoScene::DrawObjsRecursive(PagShaderProgram* shader, NodoScene* nodo, const TypeDraw& type)
{
	if (nodo->nodos.empty()) {
		if (!nodo->objs.empty()) {
			for (int i = 0; i < nodo->objs.size(); i++) {
				nodo->objs[i]->DrawObj(shader, type);
			}
		}
	}
	else {
		for (int i = 0; i < nodo->nodos.size(); i++) {
			DrawObjsRecursive(shader, nodo->nodos[i], type);
		}

		if (!nodo->objs.empty()) {
			for (int i = 0; i < nodo->objs.size(); i++) {
				nodo->objs[i]->DrawObj(shader, type);
			}
		}
	}
}