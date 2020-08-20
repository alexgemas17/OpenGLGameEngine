#include "Scene.h"

#include "../Input/InputManager.h"
#include "../Loaders/FileLoader.h"
#include "../Application.h"
#include <random>

#include "../BasicElement/Plane.h"

#include <../glm/gtc/type_ptr.hpp>

Scene::Scene(): camara(nullptr)
{
	glPrimitiveRestartIndex(0xFFFFFFFF); //Posible incompatibilidad con los modelos cargados desde Assimp.
	glEnable(GL_PRIMITIVE_RESTART);
	glEnable(GL_PROGRAM_POINT_SIZE);

	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glDisable(GL_BLEND); //Activar despues del Deferred rendering si se quiere usar
}

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

	LoadObjs();
	InitLights();

	InitShadowMapBuffer();
	InitGBuffer();
	InitSSAOBuffer();
	//InitDeferredLightBuffer();

	this->nodoWorld->InitObjs();
	//this->nodoLight->InitObjs();

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
	AssimpLoader* loader = new AssimpLoader();

	//this->nodoLight = new NodoScene();

	// lighting info
	// -------------
	srand(13);
	for (unsigned int i = 0; i < NR_POINT_LIGHTS; i++)
	{
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

	//Cubo Luz 
	/*for (int i = 0; i < NR_POINT_LIGHTS; i++) {

		NodoScene* cubo = loader->loadModelAssimpNode(
			Application::getInstance()->getPath() + "OpenGLEngineTFG\\BasicElement\\cube.obj", "");

		cubo->Scale(0.1f, 0.1f, 0.1f);
		cubo->Translate(lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
		cubo->setTypeRenderNode(BasicColor);
		nodoLight->addNodo(cubo);
	}*/

	delete loader;
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

/* Buffer para el Geometry Pass */
void Scene::InitGBuffer()
{
	// configure g-buffer framebuffer
	// ------------------------------
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// color + specular color buffer
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	// Info PBR: metallic, roughness, ao
	glGenTextures(1, &gMaterialInfo);
	glBindTexture(GL_TEXTURE_2D, gMaterialInfo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gMaterialInfo, 0);

	// create and attach depth buffer (renderbuffer)
	glGenTextures(1, &DepthGBuffer);
	glBindTexture(GL_TEXTURE_2D, DepthGBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthGBuffer, 0);

	/*unsigned int renderBuffer = 0;
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);*/

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(4, attachments);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/* SIN USAR. TO-DO: Ver si vale la pena */
void Scene::InitDeferredLightBuffer()
{
	//			ESTO ES POR SI LO QUIERO EN OTRO BUFFER PERO VALE LA PENA??
	//glGenFramebuffers(1, &lightBuffer);

	////  ---------------- Color texture  ---------------- 
	//glBindFramebuffer(GL_FRAMEBUFFER, lightBuffer);
	//glGenTextures(1, &ColorTextureDeferredLight);
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, ColorTextureDeferredLight);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_RGB, GL_UNSIGNED_BYTE, (const void*) nullptr);

	//// Texture wrapping
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//// Texture filtering
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//// Mipmapping --> No tiene??
	////glGenerateMipmap(GL_TEXTURE_2D);
	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, m_TextureSettings.MipBias);

	//// Anisotropic filtering (TODO: Move the anistropyAmount calculation to Defs.h to avoid querying the OpenGL driver everytime)
	//float maxAnisotropy;
	//glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	//float anistropyAmount = glm::min(maxAnisotropy, 1.0f);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anistropyAmount);*/

	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTextureDeferredLight, 0);


	////  ---------------- Detph Stencil Data  ---------------- 
	//glBindFramebuffer(GL_FRAMEBUFFER, lightBuffer);
	//glGenTextures(1, &DepthStencilTextureDeferredLight);
	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, DepthStencilTextureDeferredLight);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, (const void*) nullptr);

	//// Texture wrapping
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//// Texture filtering
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//// Mipmapping --> No tiene??
	////glGenerateMipmap(GL_TEXTURE_2D);
	////glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, m_TextureSettings.MipBias);

	//// Anisotropic filtering (TODO: Move the anistropyAmount calculation to Defs.h to avoid querying the OpenGL driver everytime)
	//float maxAnisotropy1;
	//glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy1);
	//float anistropyAmount1 = glm::min(maxAnisotropy1, 1.0f);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy1);*/

	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, DepthStencilTextureDeferredLight, 0);

	//// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	//unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0};
	//glDrawBuffers(1, attachments);

	////glDrawBuffer(GL_NONE);
	////glReadBuffer(GL_NONE);

	////finally check if framebuffer is complete
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//	std::cout << "Error, DeferredLightBuffer not complete!" << std::endl;

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
unsigned int quadVAO = 0;
unsigned int quadVBO;
// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
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

/* Llamada principal que llama a las distintas pasadas dentro del rendering */
void Scene::DrawObjs()
{
	//Datos necesarios
	SCR_WIDTH = Application::getInstance()->getWIDHT();
	SCR_HEIGHT = Application::getInstance()->getHEIGHT();

	glm::mat4 mView = camara->getView();
	glm::mat4 mProj = camara->getProjection();
	glm::mat4 mViewProjection = camara->getMatrixViewProjection();

	// 1. Shadow map
	// -----------------------------------------------------------------
	shadowMapPass();

	// 2. geometry pass: render scene's geometry/color data into gbuffer
	// -----------------------------------------------------------------
	gBufferPass(mView, mViewProjection);

	// 3. generate SSAO texture and blur SSAO texture to remove noise
	// ------------------------------------------------------------------------
	ssaoPass(mView, mProj);

	// 4. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
	// -----------------------------------------------------------------------------------------------------------------------
	deferredLightPass(mView, mProj);

	// 5. Forward rendering
	// ----------------------------------------------------------------------------------
	forwardPass(mView, mProj);

	// 6. PostProcess effects
	// ----------------------------------------------------------------------------------
	//postProcessEffectsPass(mViewProjection);
}

/* Posiciona la escena según el punto de vista de la luz direccional y la dibuja */
void Scene::shadowMapPass()
{
	glm::mat4 lightProjection, lightView;
	float near_plane = 1.0f, far_plane = 400.5f;
	//lightProjection = glm::perspective(glm::radians(60.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);
	lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);
	glm::vec3 Position = glm::vec3(0.0, 150.0f, 0.0f);
	glm::vec3 UP = glm::vec3(0.0, 1.0, 0.0);

	glm::vec3 directionalLight = glm::vec3(-0.25f, -1.0f, -0.25f);

	lightView = this->camara->getNewLookAt(Position, directionalLight + Position, UP);
	lightSpaceMatrix = shadowBias * lightProjection * lightView;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);

	// render scene from light's point of view
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMap);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	ShaderManager::getInstance()->getShadowMap()->use();
	this->nodoWorld->DrawObjs(ShaderManager::getInstance()->getShadowMap(), TypeDraw::ShadowMap);
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

/* Pasasa del SSAO */
void Scene::ssaoPass(glm::mat4& mView, glm::mat4& mProj)
{
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	ShaderManager::getInstance()->getSSAO()->use();

	ShaderManager::getInstance()->getSSAO()->setUniform("ssaoStrength", 3.0f);
	ShaderManager::getInstance()->getSSAO()->setUniform("sampleRadius", 2.0f);
	ShaderManager::getInstance()->getSSAO()->setUniform("sampleRadius2", 4.0f); //sampleRadius * sampleRadius
	ShaderManager::getInstance()->getSSAO()->setUniform("numKernelSamples", 64); //samples.size()
	ShaderManager::getInstance()->getSSAO()->setUniform("samples", 64, &ssaoKernel[0]);

	ShaderManager::getInstance()->getSSAO()->setUniform("mViewProj", mProj * mView);
	ShaderManager::getInstance()->getSSAO()->setUniform("viewInverse", glm::inverse(mView));
	ShaderManager::getInstance()->getSSAO()->setUniform("projectionInverse", glm::inverse(mProj));

	ShaderManager::getInstance()->getSSAO()->setUniform("noiseScale", glm::vec2(SCR_WIDTH * 0.25f, SCR_HEIGHT * 0.25f));
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DepthGBuffer);
	ShaderManager::getInstance()->getSSAO()->setUniform("depthTexture", 0);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	ShaderManager::getInstance()->getSSAO()->setUniform("gNormal", 1);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	ShaderManager::getInstance()->getSSAO()->setUniform("texNoise", 2);
	
	renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 3. blur SSAO texture to remove noise
	// ------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	ShaderManager::getInstance()->getSSAOBlur()->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	ShaderManager::getInstance()->getSSAOBlur()->setUniform("ssaoInput", 0); 

	ShaderManager::getInstance()->getSSAOBlur()->setUniform("numSamplesAroundTexel", 2); // 5x5 kernel blur
	renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
}

/* Pasasa del GBuffer */
void Scene::gBufferPass(glm::mat4& mView, glm::mat4& mViewProjection)
{
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	ShaderManager::getInstance()->getGBuffer()->use();
	this->nodoWorld->DrawObjs(ShaderManager::getInstance()->getGBuffer(), TypeDraw::GeometryPass);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

/* Pasasa de luces */
void Scene::deferredLightPass(glm::mat4& mView, glm::mat4& mProj)
{
	//No hacemos glBindFramebuffer porque estamos usando el deafult buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);

	//Pasamos los datos importantes al DeferredLightPassShader and bind GBuffer data
	ShaderManager::getInstance()->getDeferredShading()->use();

	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gPosition", 0);*/
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DepthGBuffer);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("depthTexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gNormal", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gAlbedo", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gMaterialInfo);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gMaterialInfo", 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("ssaoTexture", 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, DepthShadowMap);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("shadowMap", 5);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("lightSpaceMatrix", lightSpaceMatrix);
	
	ShaderManager::getInstance()->getDeferredShading()->setUniform("viewPosition", this->camara->getPosition());
	ShaderManager::getInstance()->getDeferredShading()->setUniform("viewInverse", glm::inverse(mView));
	ShaderManager::getInstance()->getDeferredShading()->setUniform("projectionInverse", glm::inverse(mProj));

	// ------------------------ Light Pass ------------------------
	//NOTA: Las luces están dentro del shader. TO-DO: Ver como pasarla aquí para ir de una en una.
	ShaderManager::getInstance()->getDeferredShading()->setUniform("NumPointLights", NR_POINT_LIGHTS);

	// Directional Light 
	ShaderManager::getInstance()->getDeferredShading()->setUniform("DirLight.LightDirection", glm::vec3(-1.0f, -1.0f, -1.0f));
	ShaderManager::getInstance()->getDeferredShading()->setUniform("DirLight.Intensity", 2.0f);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("DirLight.LightColour", glm::vec3(1.0f));

	// Point Light
	for (int i = 0; i < NR_POINT_LIGHTS; i++) {
		ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].Position", lightPositions[i]);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].Intensity", 10.0f);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].LightColour", glm::vec3(1.0f));
		ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].AttenuationRadius", 30.0f);
	}

	//Spot Light 
	ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.Position", glm::vec3(0.0, 10.0, 5.0));
	ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.LightDirection", glm::vec3(0.0f, -1.0f, 0.0f));

	ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.Intensity", 100.0f);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.LightColour", glm::vec3(1.0f, 0.52, 0.0));
	ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.AttenuationRadius", 50.0f);

	ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.CutOff", glm::cos(glm::radians(20.0f)));
	ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.OuterCutOff", glm::cos(glm::radians(0.0f)));

	renderQuad();

	glEnable(GL_DEPTH_TEST);
}

/* Pasasa de los objetos que no se han podido incluir dentro del deferred rendering: transparecias, skymap, etc... */
void Scene::forwardPass(glm::mat4 mView, glm::mat4 mProj)
{
	// Ccopy content of geometry's depth buffer to default framebuffer's depth buffer
	// ----------------------------------------------------------------------------------
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
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
	ShaderManager::getInstance()->getGodRays()->use();
	glm::vec3 lightPos = mViewProjection * glm::vec4(glm::vec3(-1.0f, -1.0f, -1.0f),1.0f);
	ShaderManager::getInstance()->getGodRays()->setUniform("ScreenLightPos", lightPos);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	ShaderManager::getInstance()->getGodRays()->setUniform("input_texture", 0);
	renderQuad();

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