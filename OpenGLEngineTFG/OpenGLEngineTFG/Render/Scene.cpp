#include "Scene.h"

#include "../Input/InputManager.h"
#include "../Loaders/FileLoader.h"
#include "../Application.h"

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
}

Scene::~Scene() {}

void Scene::InitObjs()
{
	//Cargamos los objetos
	AssimpLoader* loader = new AssimpLoader();

	this->nodoWorld = new NodoScene();
	this->nodoLight = new NodoScene();

	FileLoader fileLoader;
	fileLoader.readFromFile(Application::getInstance()->getPath() + "Data\\Objs.txt");

	// -------- OBJS --------
	std::vector<ObjFile> mainObjs = fileLoader.getMainScene();
	for (int i = 0; i < mainObjs.size(); i++) {
		NodoScene* nodo = loader->loadModelAssimpNode(mainObjs[i].obj, mainObjs[i].albedo, mainObjs[i].normal_mapping, mainObjs[i].material);
		nodo->Rotate(mainObjs[i].angleRotation, mainObjs[i].rotationDirection);
		nodo->Scale(mainObjs[i].scale.x, mainObjs[i].scale.y, mainObjs[i].scale.z);
		nodo->Translate(mainObjs[i].position.x, mainObjs[i].position.y, mainObjs[i].position.z);

		nodoWorld->addNodo(nodo);
	}

	//-------- LIGHTS --------
	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	//Cubo Luz 
	NodoScene* cubo1 = loader->loadModelAssimpNode(Application::getInstance()->getPath() + "OpenGLEngineTFG\\BasicElement\\cube.obj", "", "", "");
	cubo1->Scale(0.7f, 0.7f, 0.7f);
	cubo1->Translate(pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
	cubo1->setTypeRenderNode(BasicColor);
	nodoLight->addNodo(cubo1);

	NodoScene* cubo2 = loader->loadModelAssimpNode(Application::getInstance()->getPath() + "OpenGLEngineTFG\\BasicElement\\cube.obj", "", "", "");
	cubo2->Scale(0.7f, 0.7f, 0.7f);
	cubo2->Translate(pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
	cubo2->setTypeRenderNode(BasicColor);
	nodoLight->addNodo(cubo2);

	NodoScene* cubo3 = loader->loadModelAssimpNode(Application::getInstance()->getPath() + "OpenGLEngineTFG\\BasicElement\\cube.obj", "", "", "");
	cubo3->Scale(0.7f, 0.7f, 0.7f);
	cubo3->Translate(pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
	cubo3->setTypeRenderNode(BasicColor);
	nodoLight->addNodo(cubo3);

	NodoScene* cubo4 = loader->loadModelAssimpNode(Application::getInstance()->getPath() + "OpenGLEngineTFG\\BasicElement\\cube.obj", "", "", "");
	cubo4->Scale(0.7f, 0.7f, 0.7f);
	cubo4->Translate(pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
	cubo4->setTypeRenderNode(BasicColor);
	nodoLight->addNodo(cubo4);

	delete loader;

	// Los inicializamos
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
	if (InputManager::getInstance()->getInputButtonDown(Key_C)) {
		nodoWorld->Translate(0.0f, 0.5f, 0.0f);
	}

	if (InputManager::getInstance()->getInputButtonDown(Key_V)) {
		nodoWorld->Translate(0.0f,-0.5f,0.0f);
	}
}

void Scene::DrawObjs()
{
	//"Limpiamos" los buffers
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderManager::getInstance()->getBasicLightShader()->use();

	//ShaderManager::getInstance()->getBasicLightShader()->setUniform("light.position", normalize(glm::vec3(camara->getView() * glm::vec4(lightPosition, 0.0))));
	//ShaderManager::getInstance()->getBasicLightShader()->setUniform("light.position", lightPosition);

	//// light properties
	//ShaderManager::getInstance()->getBasicLightShader()->setUniform("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	//ShaderManager::getInstance()->getBasicLightShader()->setUniform("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	//ShaderManager::getInstance()->getBasicLightShader()->setUniform("light.specular", glm::vec3(0.4f, 0.4f, 0.4f));
	//
	//// material properties
	//ShaderManager::getInstance()->getBasicLightShader()->setUniform("material.specular", glm::vec3(0.7f, 0.6f, 0.6f));
	//ShaderManager::getInstance()->getBasicLightShader()->setUniform("material.shininess", 32.0f);

	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	ShaderManager::getInstance()->getBasicLightShader()->setUniform("material.shininess", 32.0f);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("viewPos", this->camara->getPosition());

	 /*
		   Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		   the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		   by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		   by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/
	// directional light
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("dirLight.ambient",   glm::vec3( 0.05f, 0.05f, 0.05f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("dirLight.diffuse",   glm::vec3( 0.4f, 0.4f, 0.4f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("dirLight.specular",  glm::vec3( 0.5f, 0.5f, 0.5f));

	// point light 1
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[0].position", pointLightPositions[0]);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[0].constant", 1.0f);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[0].linear", 0.09f);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[0].quadratic", 0.032f);

	// point light 2
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[1].position", pointLightPositions[1]);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[1].constant", 1.0f);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[1].linear", 0.09f);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[1].quadratic", 0.032f);

	// point light 3
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[2].position", pointLightPositions[2]);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[2].constant", 1.0f);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[2].linear", 0.09f);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[2].quadratic", 0.032f);

	// point light 4
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[3].position", pointLightPositions[3]);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[3].constant", 1.0f);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[3].linear", 0.09f);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("pointLights[3].quadratic", 0.032f);

	// spotLight
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("spotLight.position", glm::vec3(0, 20, 0));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("spotLight.direction", glm::vec3(0, -1, 0));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("spotLight.constant", 1.0f);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("spotLight.linear", 0.09f);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("spotLight.quadratic", 0.032f);
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	ShaderManager::getInstance()->getBasicLightShader()->setUniform("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	
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