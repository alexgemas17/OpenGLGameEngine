#include "Scene.h"

#include "../Input/InputManager.h"
#include "../Loaders/FileLoader.h"
#include "../Application.h"
#include <random>

#include "../BasicElement/Plane.h"

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

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

/* ---------------------------------------	INITS	----------------------------------------- */
void Scene::InitScene()
{
	SCR_WIDTH = Application::getInstance()->getWIDHT();
	SCR_HEIGHT = Application::getInstance()->getHEIGHT();

	/*InitShadowMapBuffer();
	InitSSAOBuffer();*/
	forwardRender->createFrameBuffer();
	deferredShadingRender->createFrameBuffer();
	forwardPlusRender->createFrameBuffer(NR_POINT_LIGHTS);

	LoadObjs();
	InitLights();

	this->nodoWorld->InitObjs();

	skybox = new CubeMap(Type1);
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
		NodoScene* nodo = loader->loadModelAssimpNode(mainObjs[i]);

		//Realizamos las transformaciones necesarias.
		nodo->Rotate(mainObjs[i].angleRotation, mainObjs[i].rotationDirection);
		nodo->Scale(mainObjs[i].scale.x, mainObjs[i].scale.y, mainObjs[i].scale.z);
		nodo->Translate(mainObjs[i].position.x, mainObjs[i].position.y, mainObjs[i].position.z);

		nodoWorld->addNodo(nodo);
	}

	//Data from floor
	Plane* floor = new Plane(Narrow1, 20.0f);
	SceneObj* obj = floor->getSceneObj();

	NodoScene* nodo = new NodoScene();
	nodo->Translate(0.0f, -1.5f, 0.0f);
	nodo->addObj(obj);

	nodoWorld->addNodo(nodo);

	delete loader;
}

/* Crea aleatoriamente un número determinado de luces*/
void Scene::InitLights()
{
	//Cargamos los objetos
	//AssimpLoader* loader = new AssimpLoader();

	// lighting info
	// -------------
	srand(13);
	for (unsigned int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		lightIntensity.push_back((RandomFloat(0.1, 0.9)));

		// calculate slightly random offsets
		float xPos = RandomFloat(-20, 20);
		float yPos = RandomFloat(-20, 20);
		float zPos = RandomFloat(-20, 20);
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

		// also calculate random color
		float rColor = RandomFloat(0.1, 1.0); // between 0.1 and 1.0
		float gColor = RandomFloat(0.1, 1.0); // between 0.1 and 1.0
		float bColor = RandomFloat(0.1, 1.0); // between 0.1 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	forwardPlusRender->initBufferLights(lightPositions, lightColors, lightIntensity);

	//delete loader;
}

/* Buffer para el ShadowMap */
void Scene::InitShadowMapBuffer()
{
	// configure depth map FBO
	// -----------------------
	glGenFramebuffers(1, &shadowMap);

	// create depth texture
	glGenTextures(1, &DepthShadowMap);
	glBindTexture(GL_TEXTURE_2D, DepthShadowMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);


	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMap);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthShadowMap, 0);

	GLenum drawBuffers[] = { GL_NONE };
	glDrawBuffers(1, drawBuffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Set shadow bias
	this->shadowBias = glm::mat4(
		glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
	);
}

/* Buffer para el SSAO */
void Scene::InitSSAOBuffer()
{
	glGenFramebuffers(1, &ssaoFBO);  
	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	// SSAO color buffer
	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Framebuffer not complete!" << std::endl;
	
	// and blur stage
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);

	glGenTextures(1, &ssaoColorBufferBlur);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ---------------------- generate sample kernel ----------------------
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
	std::default_random_engine generator;
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0;

		// scale samples s.t. they're more aligned to center of kernel
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	// ---------------------- generate noise texture ----------------------
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
		ssaoNoise.push_back(noise);
	}
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
}

/* ---------------------------------------	RENDERING PASS	----------------------------------------- */

/* Llamada principal que llama a las distintas pasadas dentro del rendering */
void Scene::DrawObjs()
{
	//Datos necesarios
	SCR_WIDTH = Application::getInstance()->getWIDHT();
	SCR_HEIGHT = Application::getInstance()->getHEIGHT();

	glm::mat4 mView = camara->getView();
	glm::mat4 mProj = camara->getProjection();
	glm::mat4 mViewProjection = camara->getMatrixViewProjection();

	if (mode == 0) {
		ShaderManager::getInstance()->getForwardLighting()->setUniform("viewPos", camara->getPosition());
		forwardRender->draw(this->nodoWorld, this->lightPositions, this->lightColors, this->lightIntensity);
	}
	else if (mode == 1) {
		deferredShadingRender->draw(this->nodoWorld, this->lightPositions, this->lightColors, this->lightIntensity);
	}
	else if (mode == 2) {
		forwardPlusRender->draw(this->nodoWorld);
	}

	skyboxRender(mView, mProj);
}

/* Posiciona la escena según el punto de vista de la luz direccional y la dibuja */
void Scene::shadowMapPass()
{
	//glm::mat4 lightProjection, lightView;
	//float near_plane = 1.0f, far_plane = 400.5f;
	////lightProjection = glm::perspective(glm::radians(60.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);
	//lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);
	//glm::vec3 Position = glm::vec3(0.0, 150.0f, 0.0f);
	//glm::vec3 UP = glm::vec3(0.0, 1.0, 0.0);

	//glm::vec3 directionalLight = glm::vec3(-0.25f, -1.0f, -0.25f);

	//lightView = this->camara->getNewLookAt(Position, directionalLight + Position, UP);
	//lightSpaceMatrix = shadowBias * lightProjection * lightView;

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	//glEnable(GL_BLEND);

	//// render scene from light's point of view
	//glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//glBindFramebuffer(GL_FRAMEBUFFER, shadowMap);
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	//ShaderManager::getInstance()->getShadowMap()->use();
	//this->nodoWorld->DrawObjs(ShaderManager::getInstance()->getShadowMap(), TypeDraw::ShadowMap);
	//glCullFace(GL_BACK);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//// reset viewport
	//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

/* Pasasa del SSAO */
void Scene::ssaoPass(glm::mat4& mView, glm::mat4& mProj)
{
	//glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	//glClear(GL_COLOR_BUFFER_BIT);

	//glDisable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	//ShaderManager::getInstance()->getSSAO()->use();

	//ShaderManager::getInstance()->getSSAO()->setUniform("ssaoStrength", 3.0f);
	//ShaderManager::getInstance()->getSSAO()->setUniform("sampleRadius", 2.0f);
	//ShaderManager::getInstance()->getSSAO()->setUniform("sampleRadius2", 4.0f); //sampleRadius * sampleRadius
	//ShaderManager::getInstance()->getSSAO()->setUniform("numKernelSamples", 64); //samples.size()
	//ShaderManager::getInstance()->getSSAO()->setUniform("samples", 64, &ssaoKernel[0]);

	//ShaderManager::getInstance()->getSSAO()->setUniform("mViewProj", mProj * mView);
	//ShaderManager::getInstance()->getSSAO()->setUniform("viewInverse", glm::inverse(mView));
	//ShaderManager::getInstance()->getSSAO()->setUniform("projectionInverse", glm::inverse(mProj));

	//ShaderManager::getInstance()->getSSAO()->setUniform("noiseScale", glm::vec2(SCR_WIDTH * 0.25f, SCR_HEIGHT * 0.25f));
	//
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, DepthGBuffer);
	//ShaderManager::getInstance()->getSSAO()->setUniform("depthTexture", 0);
	//
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, gNormal);
	//ShaderManager::getInstance()->getSSAO()->setUniform("gNormal", 1);
	//
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, noiseTexture);
	//ShaderManager::getInstance()->getSSAO()->setUniform("texNoise", 2);
	//
	//renderQuad();

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//// 3. blur SSAO texture to remove noise
	//// ------------------------------------
	//glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	//glClear(GL_COLOR_BUFFER_BIT);

	//ShaderManager::getInstance()->getSSAOBlur()->use();

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	//ShaderManager::getInstance()->getSSAOBlur()->setUniform("ssaoInput", 0); 

	//ShaderManager::getInstance()->getSSAOBlur()->setUniform("numSamplesAroundTexel", 2); // 5x5 kernel blur
	//renderQuad();

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glEnable(GL_DEPTH_TEST);
}

/* Pasasa de los objetos que no se han podido incluir dentro del deferred rendering: transparecias, skymap, etc... */
void Scene::skyboxRender(glm::mat4 mView, glm::mat4 mProj)
{
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

	//for (int i = 0; i < NR_LIGHTS; i++) {
	//this->nodoLight->DrawObjs(mView, mViewProjection);
	//}

	glDepthFunc(GL_LEQUAL);
	ShaderManager::getInstance()->getSkyBox()->use();
	ShaderManager::getInstance()->getSkyBox()->setUniform("ViewProjMatrix", mProj * glm::mat4(glm::mat3(mView)));
	skybox->Draw();
	glDepthFunc(GL_LESS); // set depth function back to default
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