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
	//glDepthFunc(GL_LEQUAL);
	glDepthFunc(GL_LESS);
	//glEnable(GL_BLEND);
	glDisable(GL_BLEND); //Activar despues del Deferred rendering si se quiere usar
}

Scene::~Scene() {}

void Scene::InitScene()
{
	LoadObjs();

	InitGBuffer();
	InitLights();

	this->nodoWorld->InitObjs();
	this->nodoLight->InitObjs();
}

void Scene::LoadObjs()
{
	//Cargamos los objetos
	AssimpLoader* loader = new AssimpLoader();

	this->nodoWorld = new NodoScene();

	FileLoader fileLoader;
	fileLoader.readFromFile(Application::getInstance()->getPath() + "Data\\Objs.txt");

	// -------- OBJS --------
	std::vector<ObjFile> mainObjs = fileLoader.getMainScene();
	for (int i = 0; i < mainObjs.size(); i++) {
		//Cargamos el objeto desde disco.
		NodoScene* nodo = loader->loadModelAssimpNode(mainObjs[i].obj, mainObjs[i].albedo, mainObjs[i].normal_mapping, mainObjs[i].material);

		//Selecionamos el tipo de rendering que queremos.
		nodo->setTypeRenderNode(DeferredRendering);

		//Realizamos las transformaciones necesarias.
		nodo->Rotate(mainObjs[i].angleRotation, mainObjs[i].rotationDirection);
		nodo->Scale(mainObjs[i].scale.x, mainObjs[i].scale.y, mainObjs[i].scale.z);
		nodo->Translate(mainObjs[i].position.x, mainObjs[i].position.y, mainObjs[i].position.z);

		nodoWorld->addNodo(nodo);
	}

	delete loader;
}

float RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void Scene::InitLights()
{
	//Cargamos los objetos
	//AssimpLoader* loader = new AssimpLoader();

	this->nodoLight = new NodoScene();

	// lighting info
	// -------------
	const unsigned int NR_LIGHTS = 32;
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = RandomFloat(-50, 50);
		float yPos = RandomFloat(-50, 50);
		float zPos = RandomFloat(-50, 50);
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.2; // between 0.2 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.2; // between 0.2 and 1.0
		float bColor = ((rand() % 100) / 200.0f) + 0.2; // between 0.2 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	ShaderManager::getInstance()->getDeferredShading()->use();
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gPosition", 0);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gNormal", 1);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gAlbedoSpec", 2);

	////Cubo Luz 
	//NodoScene* cubo1 = loader->loadModelAssimpNode(Application::getInstance()->getPath() + "OpenGLEngineTFG\\BasicElement\\cube.obj", "", "", "");
	//cubo1->Scale(0.7f, 0.7f, 0.7f);
	//cubo1->Translate(pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
	//cubo1->setTypeRenderNode(BasicColor);
	//nodoLight->addNodo(cubo1);

	//NodoScene* cubo2 = loader->loadModelAssimpNode(Application::getInstance()->getPath() + "OpenGLEngineTFG\\BasicElement\\cube.obj", "", "", "");
	//cubo2->Scale(0.7f, 0.7f, 0.7f);
	//cubo2->Translate(pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
	//cubo2->setTypeRenderNode(BasicColor);
	//nodoLight->addNodo(cubo2);

	//NodoScene* cubo3 = loader->loadModelAssimpNode(Application::getInstance()->getPath() + "OpenGLEngineTFG\\BasicElement\\cube.obj", "", "", "");
	//cubo3->Scale(0.7f, 0.7f, 0.7f);
	//cubo3->Translate(pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
	//cubo3->setTypeRenderNode(BasicColor);
	//nodoLight->addNodo(cubo3);

	//NodoScene* cubo4 = loader->loadModelAssimpNode(Application::getInstance()->getPath() + "OpenGLEngineTFG\\BasicElement\\cube.obj", "", "", "");
	//cubo4->Scale(0.7f, 0.7f, 0.7f);
	//cubo4->Translate(pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
	//cubo4->setTypeRenderNode(BasicColor);
	//nodoLight->addNodo(cubo4);

	//delete loader;
}

void Scene::InitGBuffer()
{
	// configure g-buffer framebuffer
	// ------------------------------
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Scene::DrawObjs()
{
	// Limpiamos los buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1. geometry pass: render scene's geometry/color data into gbuffer
	// -----------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 mView = camara->getView();
	glm::mat4 mViewProjection = camara->getMatrixViewProjection();

	this->nodoWorld->DrawObjs(mView, mViewProjection);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
		// -----------------------------------------------------------------------------------------------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShaderManager::getInstance()->getDeferredShading()->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	// send light relevant uniforms
	for (unsigned int i = 0; i < lightPositions.size(); i++)
	{
		ShaderManager::getInstance()->getDeferredShading()->setUniform("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("lights[" + std::to_string(i) + "].Color", lightColors[i]);

		// update attenuation parameters and calculate radius
		const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		const float linear = 0.7;
		const float quadratic = 1.8;
		ShaderManager::getInstance()->getDeferredShading()->setUniform("lights[" + std::to_string(i) + "].Linear", linear);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("lights[" + std::to_string(i) + "].Quadratic", quadratic);

		// then calculate radius of light volume/sphere
		const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
		float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("lights[" + std::to_string(i) + "].Radius", radius);
	}
	ShaderManager::getInstance()->getDeferredShading()->setUniform("viewPos", this->camara->getPosition());

	renderQuad(); //????

	// 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
	   // ----------------------------------------------------------------------------------
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
	// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
	// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
	// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
	glBlitFramebuffer(0,
		0, 
		Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(),
		0, 
		0, Application::getInstance()->getWIDHT(), 
		Application::getInstance()->getHEIGHT(), 
		GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	//this->nodoLight->DrawObjs(mView, mViewProjection);
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