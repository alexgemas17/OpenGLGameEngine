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
		4,0,5,4, 0xFFFFFFFF,
		5,0,1,5, 0xFFFFFFFF,

		0,2,1,0, 0xFFFFFFFF,
		1,2,3,1, 0xFFFFFFFF,

		5,7,4,5, 0xFFFFFFFF,
		4,7,6,4, 0xFFFFFFFF,

		4,6,0,4, 0xFFFFFFFF,
		0,6,2,0, 0xFFFFFFFF,

		1,3,5,1, 0xFFFFFFFF,
		5,3,7,5, 0xFFFFFFFF,

		2,6,3,2, 0xFFFFFFFF,
		3,6,7,3, 0xFFFFFFFF
	};

	std::vector<glm::vec2> coordenada_textura = {
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f)
	};

	// ----- Objeto 1 ----- 
	SceneObj *triangulo1 = new SceneObj(puntos, index, color, coordenada_textura, "..\\Data\\Texturas\\wall.png");
	triangulo1->Scale(0.5f, 0.5f, 0.5f);
	triangulo1->Rotate(45.0f, glm::vec3(0,1,0));
	triangulo1->Rotate(25.0f, glm::vec3(1, 0, 0));

	// ----- Objeto 2 ----- 
	SceneObj *triangulo2 = new SceneObj(puntos, index, color, coordenada_textura, "..\\Data\\Texturas\\wall.png");
	triangulo2->Scale(0.5f, 0.5f, 0.5f);
	triangulo2->Rotate(45.0f, glm::vec3(0, 0, 1));

	// ----- Nodo nivel 1 ----- 
	NodoScene *level1 = new NodoScene();
	level1->Translate(0.6f, -0.5f, 0.0f);
	level1->addObj(triangulo2);

	// ----- Nodo Raiz ----- 
	this->nodo = new NodoScene();
	nodo->addObj(triangulo1);
	nodo->addNodo(level1);
}

Scene::~Scene() {}

void Scene::InitObjs()
{
	this->nodo->InitObjs();
}

void Scene::InitCamara(float fov, int width, int height, float zNear, float zFar)
{
	this->camara = new Camara(fov, width, height, zNear, zFar);
}

/* Recorremos los objetos que necesiten actualizar su estado */
void Scene::UpdateObjs()
{
	camara->UpdateCamera();

	//Inputs de prueba
	/*if (InputManager::getInstance()->getInputButtonDown(Key_A)) {
		nodo->Translate(0.5f, 0.0f, 0.0f);
	}

	if (InputManager::getInstance()->getInputButtonDown(Key_D)) {
		nodo->Translate(-0.5f,0.0f,0.0f);
	}*/

	nodo->Rotate((float)glfwGetTime() * 0.2f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Scene::DrawObjs(PagShaderProgram* shader)
{
	//"Limpiamos" los buffers
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Dibujamos los objetos
	this->nodo->DrawObjs(shader, camara->getMatrixViewProjection());
}

/* Funciones callbacks */
void Scene::framebuffer_size_callback(int width, int height)
{
	glViewport(0, 0, width, height);
	this->camara->SetProjection(width, height);
}

void Scene::mouse_button_callback(int button, int action, int mods)
{
	//TODO
}

void Scene::cursor_position_callback(double xpos, double ypos)
{
	this->camara->moveCamara(xpos, ypos);
}

void Scene::scroll_callback(double xoffset, double yoffset)
{
	//TODO
}
