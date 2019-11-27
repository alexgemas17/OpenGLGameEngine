#include "Scene.h"

#include "../Input/InputManager.h"

Scene::Scene(): camara(nullptr)
{
	glPrimitiveRestartIndex(0xFFFFFFFF);
	glEnable(GL_PRIMITIVE_RESTART);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);


	// ----- Objeto 1 ----- 
	//SceneObj *triangulo1 = new SceneObj(puntos, index, color, coordenada_textura, "..\\Data\\Texturas\\wall.png");
	//triangulo1->Scale(0.5f, 0.5f, 0.5f);
	//triangulo1->Rotate(45.0f, glm::vec3(0,1,0));
	//triangulo1->Rotate(25.0f, glm::vec3(1, 0, 0));

	//// ----- Nodo Raiz ----- 
	//this->nodo = new NodoScene();
	//nodo->addObj(triangulo1);
	//nodo->addNodo(level1);
	AssimpLoader* loader = new AssimpLoader();

	this->nodoWorld = new NodoScene();
	this->nodoLight = new NodoScene();
	
	//NodoScene *nodo1 = loader->loadModelAssimp("D:\\Proyectos\\MODELOS_TFG\\Japanese_Temple_Model\\Model\\Japanese_Temple.fbx");
	//nodo1->Rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	//nodo->addNodo(nodo1);

	//Cubo 1 
	Cube* cubo = new Cube(0.2f);
	cubo->getSceneObj()->Scale(0.7f, 0.7f, 0.7f);
	cubo->getSceneObj()->Rotate(15, glm::vec3(1.0f, 0.0f, 0.0f));
	nodoWorld->addObj(cubo->getSceneObj());


	lightPosition = glm::vec3(2.2f, 5.0f, 3.0f);

	//Cubo Luz 
	Cube* cubo2 = new Cube(0.1f);
	cubo2->getSceneObj()->setTypeRender(BasicColor);
	cubo2->getSceneObj()->Scale(0.8f, 0.8f, 0.8f);
	cubo2->getSceneObj()->Translate(lightPosition.x, lightPosition.y, lightPosition.z);
	nodoLight->addObj(cubo2->getSceneObj());

	delete loader;
}

Scene::~Scene() {}

void Scene::InitObjs()
{
	this->nodoWorld->InitObjs();
	this->nodoLight->InitObjs();
}

void Scene::InitCamara(float fov, int width, int height, float zNear, float zFar)
{
	this->camara = new Camara(fov, width, height, zNear, zFar);
}

/* Recorremos los objetos que necesiten actualizar su estado */
void Scene::UpdateObjs(float deltaTime)
{
	camara->UpdateCamera(deltaTime);

	//Inputs de prueba
	/*if (InputManager::getInstance()->getInputButtonDown(Key_A)) {
		nodo->Translate(0.5f, 0.0f, 0.0f);
	}

	if (InputManager::getInstance()->getInputButtonDown(Key_D)) {
		nodo->Translate(-0.5f,0.0f,0.0f);
	}*/
}

void Scene::DrawObjs()
{
	//"Limpiamos" los buffers
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderManager::getInstance()->getBasicLightShader()->use();

	ShaderManager::getInstance()->getBasicLightShader()->setUniform("light.position", normalize(glm::vec3(camara->getView() * glm::vec4(lightPosition, 0.0))));
	//ShaderManager::getInstance()->getBasicLightShader()->setUniform("light.position", lightPosition);

	ShaderManager::getInstance()->getBasicLightShader()->setUniform("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("light.specular", glm::vec3(0.4f, 0.4f, 0.4f));

	ShaderManager::getInstance()->getBasicLightShader()->setUniform("material.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("material.diffuse", glm::vec3(0.5f, 0.0f, 0.0f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("material.specular", glm::vec3(0.7f, 0.6f, 0.6f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("material.shininess", 32.0f);
	
	//Dibujamos los objetos
	glm::mat4 mView = camara->getView();
	glm::mat4 mViewProjection = camara->getMatrixViewProjection();
	this->nodoWorld->DrawObjs(mView, mViewProjection);
	this->nodoLight->DrawObjs(mView, mViewProjection);
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
