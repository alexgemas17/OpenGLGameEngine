#include "Scene.h"

#include "../Input/InputManager.h"

Scene::Scene() 
{
	glPrimitiveRestartIndex(0xFFFFFFFF);
	glEnable(GL_PRIMITIVE_RESTART);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);

	std::vector<glm::vec3> puntos_plano = {
		glm::vec3(0.5f,  0.5f, 0.0f),
		glm::vec3(0.5f, -0.5f, 0.0f),
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3(-0.5f, 0.5f, 0.0f)
	};

	std::vector<GLuint> index_plano = {
		0,2,1,3,2,0,0xFFFFFFFF
	};

	//No se usa ahora mismo
	std::vector<glm::vec3> color = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 0.0f)
	};

	std::vector<glm::vec2> coordenada_textura_plano = {
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f)
	};

	//-------------- CUBO ------------------
	std::vector<glm::vec3> puntos = {
		glm::vec3( -0.5f,  0.5f,  0.5f ),
		glm::vec3(  0.5f,  0.5f,  0.5f ),
		glm::vec3( -0.5f, -0.5f,  0.5f ),
		glm::vec3(  0.5f, -0.5f,  0.5f ),
		glm::vec3( -0.5f,  0.5f, -0.5f ),
		glm::vec3(  0.5f,  0.5f, -0.5f ),
		glm::vec3( -0.5f, -0.5f, -0.5f ),
		glm::vec3(  0.5f, -0.5f, -0.5f ),
	};

	std::vector<GLuint> index = {
		2,0,5,0xFFFFFFFF,
		5,0,1,0xFFFFFFFF,

		0,2,1,0xFFFFFFFF,
		1,2,3,0xFFFFFFFF,

		5,7,4,0xFFFFFFFF,
		4,7,6,0xFFFFFFFF,

		4,6,0,0xFFFFFFFF,
		0,6,2,0xFFFFFFFF,

		1,3,5,0xFFFFFFFF,
		5,3,7,0xFFFFFFFF,

		2,6,3,0xFFFFFFFF,
		3,6,7,0xFFFFFFFF
	};

	std::vector<glm::vec2> coordenada_textura = {
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f)
	};

	this->triangulo = new Render(puntos, index, color, coordenada_textura,"..\\Data\\Texturas\\wall.png");

	triangulo2 = new SceneObj(puntos, index, color, coordenada_textura, "..\\Data\\Texturas\\wall.png");
	triangulo2->Scale(0.5f, 0.5f, 0.5f);
}

Scene::~Scene() {}

void Scene::InitObjs()
{
	this->triangulo2->Init();
	//this->triangulo->Init();
}

/* Recorremos los objetos que necesiten actualizar su estado */
void Scene::UpdateObjs()
{
	//Inputs de prueba
	if (InputManager::getInstance()->getInputButtonDown(Key_A)) {
		triangulo2->Rotate(45.0f, glm::vec3(0,0,1));
	}
}

void Scene::DrawObjs(PagShaderProgram* shader)
{
	//"Limpiamos" los buffers
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Dibujamos los objetos
	this->triangulo2->DrawObj(shader);
}
