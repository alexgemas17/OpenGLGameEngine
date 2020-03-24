#include "Scene.h"

#include "../Input/InputManager.h"
#include "../Loaders/FileLoader.h"
#include "../Application.h"

Scene::Scene(): camara(nullptr)
{
	glPrimitiveRestartIndex(0xFFFFFFFF); //Posible incompatibilidad con los modelos cargados desde Assimp.
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
		NodoScene* nodo = loader->loadModelAssimpNode(mainObjs[i].obj, mainObjs[i].albedo);

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
	AssimpLoader* loader = new AssimpLoader();

	this->nodoLight = new NodoScene();

	// lighting info
	// -------------
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = RandomFloat(-30, 30);
		float yPos = RandomFloat(20, 50);
		float zPos = RandomFloat(-30, 30);
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

		// also calculate random color
		float rColor = RandomFloat(0.1, 1.0); // between 0.1 and 1.0
		float gColor = RandomFloat(0.1, 1.0); // between 0.1 and 1.0
		float bColor = RandomFloat(0.1, 1.0); // between 0.1 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	ShaderManager::getInstance()->getDeferredShading()->use();
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gPosition", 0);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gNormal", 1);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gAlbedoSpec", 2);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("gNormalSpec", 3);

	//ShaderManager::getInstance()->getDeferredShading()->setUniform("gTvector", 4);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("gNvector", 5);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("gBvector", 6);

	//Cubo Luz 
	for (int i = 0; i < NR_LIGHTS; i++) {

		NodoScene* cubo = loader->loadModelAssimpNode(
			Application::getInstance()->getPath() + "OpenGLEngineTFG\\BasicElement\\cube.obj", "");

		cubo->Scale(0.1f, 0.1f, 0.1f);
		cubo->Translate(lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
		cubo->setTypeRenderNode(BasicColor);
		nodoLight->addNodo(cubo);
	}

	delete loader;
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

	//// normal texture buffer
	//glGenTextures(1, &gNormalSpec);
	//glBindTexture(GL_TEXTURE_2D, gNormalSpec);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gNormalSpec, 0);

	//// normal color buffer
	//glGenTextures(1, &gTvector);
	//glBindTexture(GL_TEXTURE_2D, gTvector);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_RGB, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gTvector, 0);

	//// normal color buffer
	//glGenTextures(1, &gNvector);
	//glBindTexture(GL_TEXTURE_2D, gNvector);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_RGB, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, gNvector, 0);

	//// normal color buffer
	//glGenTextures(1, &gBvector);
	//glBindTexture(GL_TEXTURE_2D, gBvector);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_RGB, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, gBvector, 0);

	//// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	//unsigned int attachments[7] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6};
	//glDrawBuffers(7, attachments);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
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
	/*glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gNormalSpec);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gTvector);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, gNvector);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, gBvector);*/
	// send light relevant uniforms
	for (unsigned int i = 0; i < lightPositions.size(); i++)
	{
		ShaderManager::getInstance()->getDeferredShading()->setUniform("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("lights[" + std::to_string(i) + "].Color", lightColors[i]);

		// update attenuation parameters and calculate radius
		const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		const float linear = 1.7; //0.7
		const float quadratic = 2.8; //1.8
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
	
	//Dibujamos los cubos como siempre (Forward rendering)
	/*for (int i = 0; i < NR_LIGHTS; i++) {
		this->nodoLight->DrawObjs(mView, mViewProjection);
	}*/
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