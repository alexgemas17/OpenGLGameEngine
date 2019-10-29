#include "Scene.h"

Scene::Scene() {}

Scene::~Scene() {}

void Scene::InitObjs()
{
	this->verticesPrueba = {
				glm::vec3(-0.5f, -0.5f, 0.0f),  //Izq
				glm::vec3(0.5f, -0.5f, 0.0f),	//Der
				glm::vec3(0.0f, 0.5f, 0.0f)		//Arriba
	};

	this->triangulo = new Render(verticesPrueba);
	this->triangulo->Init();
}

/* Recorremos los objetos que necesiten actualizar su estado */
void Scene::UpdateObjs()
{
	//TODO
}

void Scene::DrawObjs(PagShaderProgram* shader)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	this->triangulo->Draw(shader);
}
