#include "Scene.h"

#include "../Input/InputManager.h"
#include "../Loaders/FileLoader.h"
#include "../Application.h"
#include <random>

#include "../BasicElement/Plane.h"
#include "../BasicElement/Cube.h"

#include <../glm/gtc/type_ptr.hpp>

Scene::Scene(): 
	camara(nullptr), mode(0), forwardRender(new ForwardRender()), 
	deferredShadingRender(new DeferredShadingRender()), forwardPlusRender(new ForwardPlusRender()),
	NUM_LIGHTS(200), NUM_TRANSP_OBJS(10), gamma(0.8f), exposure(1.0f)
{}

Scene::~Scene() {}

float RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

/* ---------------------------------------	INITS	----------------------------------------- */
void Scene::InitScene()
{
	SCR_WIDTH = Application::getInstance()->getWIDHT();
	SCR_HEIGHT = Application::getInstance()->getHEIGHT();

	// ---------------------------------------------------------------------------------------------------
	// configure floating point framebuffer
	// -----------------------------------
	glGenFramebuffers(1, &sceneFBO);

	// create floating point color buffer
	glGenTextures(1, &sceneColorBuffer);
	glBindTexture(GL_TEXTURE_2D, sceneColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// create depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	// attach buffers
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneColorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ---------------------------------------------------------------------------------------------------

	forwardRender->createFrameBuffer(MAX_LIGHTS);
	deferredShadingRender->createFrameBuffer(MAX_LIGHTS);
	forwardPlusRender->createFrameBuffer(MAX_LIGHTS);

	LoadObjs();
	InitLights();

	this->nodoWorld->InitObjs();
	this->transparentObj->InitObjs();

	skybox = new CubeMap(Type1); 
	cube = new Cube(10.0f);
}

void Scene::LoadObjs()
{
	//Cargamos los objetos
	AssimpLoader* loader = new AssimpLoader();

	this->nodoWorld = new NodoScene();
	this->transparentObj = new NodoScene();

	FileLoader fileLoader;
	fileLoader.readFromFile(Application::getInstance()->getPath() + "Data\\Objs.txt");

	// -------- OBJS --------
	std::vector<ObjFile> mainObjs = fileLoader.getMainScene();
	for (int i = 0; i < mainObjs.size(); i++) {
		//Cargamos el objeto desde disco.
		NodoScene* nodo = loader->loadModelAssimpNode(mainObjs[i]);

		//Realizamos las transformaciones necesarias.
		nodo->Rotate(mainObjs[i].angleRotation, mainObjs[i].rotationDirection);
		nodo->Scale(mainObjs[i].scale.x, mainObjs[i].scale.y, mainObjs[i].scale.z);
		nodo->Translate(mainObjs[i].position.x, mainObjs[i].position.y, mainObjs[i].position.z);

		nodoWorld->addNodo(nodo);
	}

	for (int i = 0; i < MAX_TRANSP_OBJS; i++) {
		// calculate slightly random offsets
		float xPos = RandomFloat(-40, 40);
		float yPos = RandomFloat(-20, 70);
		float zPos = RandomFloat(-60, 60);
		transparentObjPositions.push_back(glm::vec3(xPos, yPos, zPos));
	}

	delete loader;
}

/* Crea aleatoriamente un número determinado de luces*/
void Scene::InitLights()
{
	// lighting info
	// -------------
	srand(13);
	for (unsigned int i = 0; i < MAX_LIGHTS; i++)
	{
		lightIntensity.push_back((RandomFloat(0.2f, 1.5f)));

		// calculate slightly random offsets
		float xPos = RandomFloat(-40, 40);
		float yPos = RandomFloat(-20, 70);
		float zPos = RandomFloat(-60, 60);
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

		// also calculate random color
		float rColor = RandomFloat(0.1, 1.0); // between 0.1 and 1.0
		float gColor = RandomFloat(0.1, 1.0); // between 0.1 and 1.0
		float bColor = RandomFloat(0.1, 1.0); // between 0.1 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	forwardRender->initBufferLights(lightPositions, lightColors, lightIntensity);
	deferredShadingRender->initBufferLights(lightPositions, lightColors, lightIntensity);
	forwardPlusRender->initBufferLights(lightPositions, lightColors, lightIntensity);
}

/* Inicializa la cámara */
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

	UpdateLights(deltaTime);
	forwardRender->UpdateLights(this->lightPositions, NUM_LIGHTS);
	deferredShadingRender->UpdateLights(this->lightPositions, NUM_LIGHTS);
	forwardPlusRender->UpdateLights(this->lightPositions, NUM_LIGHTS);
}

void Scene::UpdateLights(float deltaTime)
{
	// calculate slightly random offsets
	float min = -20.0f;
	float max = 90.0f;
	float velocidad = 5.5f;

	srand(13);
	for (unsigned int i = 0; i < this->NUM_LIGHTS; i++)
	{
		lightPositions[i].y = lightPositions[i].y - velocidad * deltaTime;

		if (lightPositions[i].y < min)
			lightPositions[i].y = max;

		//lightPositions[i].y = fmod((lightPositions[i].y + (-4.5f * deltaTime) - min + max), max) + min;
	}
}

void Scene::AddNewNumLights(int numLight)
{
	if (numLight > 2500 || numLight < 1)
		return;

	this->NUM_LIGHTS = numLight;

	// --------------------------- FORWARD  RENDERING ----------------------------
	ShaderManager::getInstance()->getForwardLighting()->use();
	ShaderManager::getInstance()->getForwardLighting()->setUniform("lightCount", NUM_LIGHTS);

	// --------------------------- DEFERRED RENDERING ----------------------------
	ShaderManager::getInstance()->getDeferredShading()->use();
	ShaderManager::getInstance()->getDeferredShading()->setUniform("lightCount", NUM_LIGHTS);

	// --------------------------- FORWARD PLUS RENDERING ----------------------------
	ShaderManager::getInstance()->getLightingCulling()->use();
	ShaderManager::getInstance()->getLightingCulling()->setUniform("lightCount", NUM_LIGHTS);

	ShaderManager::getInstance()->getForwardPlusLighting()->use();
	ShaderManager::getInstance()->getForwardPlusLighting()->setUniform("lightCount", NUM_LIGHTS);
}

void Scene::SetUniforms()
{
	float workGroupsX = Application::getInstance()->getMainScene()->forwardPlusRender->getWorkGroupsX();
	int numLights = this->NUM_LIGHTS;
	int SCR_WIDTH = Application::getInstance()->getWIDHT();
	int SCR_HEIGHT = Application::getInstance()->getHEIGHT();

	// --------------------------- FORWARD  RENDERING ----------------------------
	ShaderManager::getInstance()->getForwardLighting()->use();
	ShaderManager::getInstance()->getForwardLighting()->setUniform("lightCount", numLights);
	ShaderManager::getInstance()->getForwardLighting()->setUniform("texture_albedo", 0);
	ShaderManager::getInstance()->getForwardLighting()->setUniform("texture_specular", 1);
	ShaderManager::getInstance()->getForwardLighting()->setUniform("texture_normal", 2);

	// --------------------------- DEFERRED RENDERING ----------------------------
	ShaderManager::getInstance()->getGBuffer()->use();
	ShaderManager::getInstance()->getGBuffer()->setUniform("texture_albedo", 0);
	ShaderManager::getInstance()->getGBuffer()->setUniform("texture_specular", 1);
	ShaderManager::getInstance()->getGBuffer()->setUniform("texture_normal", 2);

	ShaderManager::getInstance()->getDeferredShading()->use();
	ShaderManager::getInstance()->getDeferredShading()->setUniform("lightCount", numLights);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gPosition", 0);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gNormal", 1);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gAlbedoSpec", 2);

	// --------------------------- FORWARD PLUS RENDERING ----------------------------
	ShaderManager::getInstance()->getLightingCulling()->use();
	ShaderManager::getInstance()->getLightingCulling()->setUniform("screenSize", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
	ShaderManager::getInstance()->getLightingCulling()->setUniform("depthMap", 4);
	ShaderManager::getInstance()->getLightingCulling()->setUniform("lightCount", numLights);

	ShaderManager::getInstance()->getForwardPlusLighting()->use();
	ShaderManager::getInstance()->getForwardPlusLighting()->setUniform("lightCount", numLights);
	ShaderManager::getInstance()->getForwardPlusLighting()->setUniform("numberOfTilesX", workGroupsX);
	ShaderManager::getInstance()->getForwardPlusLighting()->setUniform("texture_albedo", 0);
	ShaderManager::getInstance()->getForwardPlusLighting()->setUniform("texture_specular", 1);
	ShaderManager::getInstance()->getForwardPlusLighting()->setUniform("texture_normal", 2);

	this->nodoWorld->setUniforms();
}

void Scene::setMode(int mode)
{
	this->mode = mode;
}

/* ---------------------------------------	RENDERING PASS	----------------------------------------- */
/* Llamada principal que llama a las distintas pasadas dentro del rendering */
void Scene::DrawObjs()
{	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, sceneFBO);
	if (mode == 0) {
		forwardRender->draw(this->nodoWorld);
	}
	else if (mode == 1) {
		deferredShadingRender->draw(this->nodoWorld, sceneFBO);
	}
	else if (mode == 2) {
		forwardPlusRender->draw(this->nodoWorld, sceneFBO);
	}

	skyboxRender();
}

/* Pasasa de los objetos que no se han podido incluir dentro del deferred rendering: transparecias, skymap, etc... */
void Scene::skyboxRender()
{
	glm::mat4 mView = camara->getView();
	glm::mat4 mProj = camara->getProjection();

	if (mode == 1) {
		// Ccopy content of geometry's depth buffer to default framebuffer's depth buffer
		// ----------------------------------------------------------------------------------
		glBindFramebuffer(GL_READ_FRAMEBUFFER, deferredShadingRender->getGBufferID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, sceneFBO);
		// write to default framebuffer
		// blit to default framebuffer. Note that this may or may not work as the internal 
		//formats of both the FBO and default framebuffer have to match.
		// the internal formats are implementation defined. This works on all of my systems, 
		//but if it doesn't on yours you'll likely have to write to the 		
		// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal 
		//format with the FBO's internal format).

		glBlitFramebuffer(
			0, 0, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(),
			0, 0, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(),
			GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
	}

	// --------------------------- Transparent objs ----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glm::mat4 model = glm::mat4(1.0f);
	ShaderManager::getInstance()->getTransparentObj()->use();
	ShaderManager::getInstance()->getTransparentObj()->setUniform("texture1", 0);
	ShaderManager::getInstance()->getTransparentObj()->setUniform("view", mView);
	ShaderManager::getInstance()->getTransparentObj()->setUniform("projection", mProj);
	ShaderManager::getInstance()->getTransparentObj()->setUniform("model", model);
	cube->Draw();
	glDisable(GL_BLEND);

	// ------------------------------- Skybox ------------------------------------------
	glDepthFunc(GL_LEQUAL);
	ShaderManager::getInstance()->getSkyBox()->use();
	ShaderManager::getInstance()->getSkyBox()->setUniform("ViewMatrix", mView);
	ShaderManager::getInstance()->getSkyBox()->setUniform("ProjMatrix", mProj);
	skybox->Draw();
	glDepthFunc(GL_LESS);

	// ----------------------------- HDR AND GAMMA -----------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// 2. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
	// --------------------------------------------------------------------------------------------------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShaderManager::getInstance()->getHDRGAMMA()->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneColorBuffer);
	ShaderManager::getInstance()->getHDRGAMMA()->setUniform("exposure", exposure);
	ShaderManager::getInstance()->getHDRGAMMA()->setUniform("gamma", gamma);
	this->deferredShadingRender->renderQuad();
}

/* Pasasa de efectos */
void Scene::postProcessEffectsPass(glm::mat4& mViewProjection)
{
	/*ShaderManager::getInstance()->getGodRays()->use();
	glm::vec3 lightPos = mViewProjection * glm::vec4(glm::vec3(-1.0f, -1.0f, -1.0f),1.0f);
	ShaderManager::getInstance()->getGodRays()->setUniform("ScreenLightPos", lightPos);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	ShaderManager::getInstance()->getGodRays()->setUniform("input_texture", 0);
	renderQuad();*/

	//Gamma Correction!!

	//BLOOM

	//HDR

	//fxaa

	//chromaticAberration??
}

/* ----------------------------------------------------------------------------------------------------------------- */
/* Funciones callbacks */
void Scene::framebuffer_size_callback(int width, int height)
{
	glViewport(0, 0, width, height);
	this->camara->SetProjection(width, height);
}

void Scene::cursor_position_callback(double xpos, double ypos)
{
	this->camara->moveCamara(xpos, ypos);
}