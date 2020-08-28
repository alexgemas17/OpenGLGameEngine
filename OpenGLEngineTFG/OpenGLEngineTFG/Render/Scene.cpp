#include "Scene.h"

#include "../Input/InputManager.h"
#include "../Loaders/FileLoader.h"
#include "../Application.h"
#include <random>

#include "../BasicElement/Plane.h"
#include "../BasicElement/Cube.h"

#include <../glm/gtc/type_ptr.hpp>

Scene::Scene(): 
	camara(nullptr), mode(2), forwardRender(new ForwardRender()), 
	deferredShadingRender(new DeferredShadingRender()), forwardPlusRender(new ForwardPlusRender())
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

	this->NUM_LIGHTS = 20;

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

/* ---------------------------------------	RENDERING PASS	----------------------------------------- */

/* Llamada principal que llama a las distintas pasadas dentro del rendering */
void Scene::DrawObjs()
{	
	if (mode == 0) {
		forwardRender->draw(this->nodoWorld);
	}
	else if (mode == 1) {
		deferredShadingRender->draw(this->nodoWorld);
	}
	else if (mode == 2) {
		forwardPlusRender->draw(this->nodoWorld);
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
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
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

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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