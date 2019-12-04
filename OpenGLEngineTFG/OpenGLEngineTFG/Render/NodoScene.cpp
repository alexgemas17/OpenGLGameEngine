#include "NodoScene.h"
#include <gtc/matrix_transform.hpp>

NodoScene::NodoScene(): Model(), typeRender(TextureLight) {}
NodoScene::~NodoScene() {}

void NodoScene::addNodo(NodoScene* nodo)
{
	this->nodos.push_back(nodo);
}

void NodoScene::addObj(SceneObj* obj)
{
	this->objs.push_back(obj);
}

void NodoScene::DrawObjs(glm::mat4& _mView, glm::mat4& _mViewProjection)
{
	glm::mat4 model = this->getModelMatrix();
	DrawObjsRecursive(this, model, _mView, _mViewProjection);
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
					nodo->objs[i]->setTypeRender(this->typeRender);
					nodo->objs[i]->Init();
				}
			}
		}
		else {
			for (int i = 0; i < nodo->nodos.size(); i++) {
				InitObjsRecursive(nodo->nodos[i]);
			}

			if (!nodo->objs.empty()) {
				for (int i = 0; i < nodo->objs.size(); i++) {
					nodo->objs[i]->setTypeRender(this->typeRender);
					nodo->objs[i]->Init();
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

void NodoScene::DrawObjsRecursive(NodoScene* nodo, glm::mat4& _modelMatrix, glm::mat4& _mView, glm::mat4& _mViewProjection)
{
	if (nodo->nodos.empty()) {
		if (!nodo->objs.empty()) {
			for (int i = 0; i < nodo->objs.size(); i++) {
				glm::mat4 model = _modelMatrix * nodo->objs[i]->getModelMatrix();
				nodo->objs[i]->DrawObj(model, _mView, _mViewProjection);
			}
		}
	}
	else {
		for (int i = 0; i < nodo->nodos.size(); i++) {
			glm::mat4 model = _modelMatrix * nodo->nodos[i]->getModelMatrix();
			DrawObjsRecursive(nodo->nodos[i], model, _mView, _mViewProjection);
		}

		if (!nodo->objs.empty()) {
			for (int i = 0; i < nodo->objs.size(); i++) {
				glm::mat4 model = _modelMatrix * nodo->objs[i]->getModelMatrix();
				nodo->objs[i]->DrawObj(model, _mView, _mViewProjection);
			}
		}
	}
}