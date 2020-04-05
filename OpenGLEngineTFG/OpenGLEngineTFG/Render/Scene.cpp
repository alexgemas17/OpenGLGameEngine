#include "Scene.h"

#include "../Input/InputManager.h"
#include "../Loaders/FileLoader.h"
#include "../Application.h"

#include "../BasicElement/Plane.h"

#include <../glm/gtc/type_ptr.hpp>

Scene::Scene(): camara(nullptr)
{
	glPrimitiveRestartIndex(0xFFFFFFFF); //Posible incompatibilidad con los modelos cargados desde Assimp.
	glEnable(GL_PRIMITIVE_RESTART);
	glEnable(GL_PROGRAM_POINT_SIZE);

	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	//glDepthFunc(GL_LESS);
	//glEnable(GL_BLEND);
	glDisable(GL_BLEND); //Activar despues del Deferred rendering si se quiere usar
}

Scene::~Scene() {}

void Scene::InitScene()
{
	LoadObjs();
	InitLights();

	//InitShadowMapBuffer();
	InitGBuffer();
	//InitDeferredLightBuffer();

	this->nodoWorld->InitObjs();
	//this->nodoLight->InitObjs();
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

		//Selecionamos el tipo de rendering que queremos.
		nodo->setTypeRenderNode(DeferredRendering);

		//Realizamos las transformaciones necesarias.
		nodo->Rotate(mainObjs[i].angleRotation, mainObjs[i].rotationDirection);
		nodo->Scale(mainObjs[i].scale.x, mainObjs[i].scale.y, mainObjs[i].scale.z);
		nodo->Translate(mainObjs[i].position.x, mainObjs[i].position.y, mainObjs[i].position.z);

		nodoWorld->addNodo(nodo);
	}

	//Data from floor
	Plane* floor = new Plane(10.0f);
	SceneObj* obj = floor->getSceneObj();

	NodoScene* nodo = new NodoScene();
	nodo->Translate(0.0f, -2.0f, 0.0f);
	nodo->Translate(0.0f, -2.0f, 0.0f);
	nodo->addObj(obj);

	nodoWorld->addNodo(nodo);

	delete loader;
}

float RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
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

/* Crear el buffer para poder calcular las sombras */
void Scene::InitShadowMapBuffer()
{
	//// configure depth map FBO
	//// -----------------------
	//glGenFramebuffers(1, &shadowMapFBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

	////  ---------------- Detph Stencil Data  ---------------- 
	//glGenTextures(1, &DepthStencilTextureShadowMap);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, DepthStencilTextureShadowMap);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, (const void*) nullptr);

	//// Texture wrapping
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	////glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));

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

	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthStencilTextureShadowMap, 0);

	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);*/

	//// finally check if framebuffer is complete
	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//	std::cout << "Error, ShadowBuffer not complete!" << std::endl;

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/* Crear el buffer donde se guardarán los datos que se pasará posteriormente a la pasada de luces*/
void Scene::InitGBuffer()
{
	int SCR_WIDTH = Application::getInstance()->getWIDHT();
	int SCR_HEIGHT = Application::getInstance()->getHEIGHT();

	// configure g-buffer framebuffer
	// ------------------------------
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// position color buffer
	glGenTextures(1, &gb_Position);
	glBindTexture(GL_TEXTURE_2D, gb_Position);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gb_Position, 0);

	// normal color buffer
	glGenTextures(1, &gb_Normal);
	glBindTexture(GL_TEXTURE_2D, gb_Normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gb_Normal, 0);

	// color + specular color buffer
	glGenTextures(1, &gb_Albedo);
	glBindTexture(GL_TEXTURE_2D, gb_Albedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gb_Albedo, 0);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
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

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
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

void Scene::shadowMapPass()
{
	//// 1. render depth of scene to texture (from light's perspective)
	//// --------------------------------------------------------------

	////lightSpaceMatrix = lightProjection * lightView
	//glm::mat4 lightSpaceMatrix = this->camara->getProjection() * this->camara->getView();

	//glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//ShaderManager::getInstance()->getShadowMap()->use();

	////LUZ DIRECCIONAL
	//glm::vec3 dirLightShadowmapLookAtPos = this->camara->getPosition() + (glm::normalize(this->camara->getFront()) * 50.0f);
	//glm::vec3 dirLightShadowmapEyePos = dirLightShadowmapLookAtPos + (-glm::vec3(-0.25f, -1.0f, -0.25f) * 100.0f);
	//glm::mat4 directionalLightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 400.0f);
	//glm::mat4 directionalLightView = glm::lookAt(dirLightShadowmapEyePos, dirLightShadowmapLookAtPos, glm::vec3(0.0f, 1.0f, 0.0f));
	//directionalLightViewProjMatrix = directionalLightProjection * directionalLightView;
	//ShaderManager::getInstance()->getShadowMap()->setUniform("lightSpaceViewProjectionMatrix", directionalLightViewProjMatrix);

	//// Render models
	//glEnable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	//glDisable(GL_CULL_FACE);

	////PASAR UNIFORM MODELMATRIX 
	//this->nodoWorld->DrawObjsShadowMap();

	//// Render terrain
	////this->nodoTerreno->DrawObjs(mView, mViewProjection);

	//// reset viewport
	//glViewport(0, 0, WIDHT, HEIGHT);
}

void Scene::gBufferPass(glm::mat4& mView, glm::mat4& mViewProjection)
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Pasamos los datos importantes al GBufferShader
	ShaderManager::getInstance()->getGBuffer()->use();
	ShaderManager::getInstance()->getGBuffer()->setUniform("ViewProjMatrix", this->camara->getMatrixViewProjection());

	this->nodoWorld->DrawObjs(ShaderManager::getInstance()->getGBuffer());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::deferredLightPass()
{
	//No hacemos glBindFramebuffer porque estamos usando el deafult buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Pasamos los datos importantes al DeferredLightPassShader and bind GBuffer data
	ShaderManager::getInstance()->getDeferredShading()->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gb_Position);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gPosition", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gb_Normal);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gNormal", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gb_Albedo);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("gAlbedo", 2);


	// ------------------------ Light Pass ------------------------
	//NOTA: Las luces están dentro del shader. TO-DO: Ver como pasarla aquí para ir de una en una.
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("numDirLights", NR_DIRECTIONAL_LIGHTS);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("nr_point_lights", NR_POINT_LIGHTS);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("numtSpotLights", NR_SPOT_LIGHTS);

	// directional light = 1
	ShaderManager::getInstance()->getDeferredShading()->setUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	ShaderManager::getInstance()->getDeferredShading()->setUniform("dirLight.shininess", 32.0f);

	ShaderManager::getInstance()->getDeferredShading()->setUniform("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	ShaderManager::getInstance()->getDeferredShading()->setUniform("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	ShaderManager::getInstance()->getDeferredShading()->setUniform("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

	const float constant = 1.0f;
	const float linear = 0.9;
	const float quadratic = 0.032;
	for (int i = 0; i < NR_POINT_LIGHTS; i++) {
		ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].position", lightPositions[i]);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].shininess", 32.0f);
		//ShaderManager::getInstance()->getDeferredShading()->setUniform("lights[" + std::to_string(i) + "].Color", lightColors[i]);

		ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
		ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].specular", glm::vec3(0.5f, 0.5f, 0.5f));

		// update attenuation parameters and calculate radius
		ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].constant", constant);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].linear", linear);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("pointLights[" + std::to_string(i) + "].quadratic", quadratic);
	}

	//for (int i = 0; i < NR_SPOT_LIGHTS; i++) {		//Solo hay 1
	//	ShaderManager::getInstance()->getDeferredShading()->setUniform(("spotLights[" + std::to_string(i) + "].intensity").c_str(), 100.0f);
	//	ShaderManager::getInstance()->getDeferredShading()->setUniform(("spotLights[" + std::to_string(i) + "].lightColour").c_str(), glm::vec3(1.0f, 1.0f, 1.0f));

		ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.position", glm::vec3(0.0f, 3.0f, 0.0f));
		ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.shininess", 32.0f);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));

		ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

		ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.constant", 1.0f);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.linear", 0.09f);
		ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.quadratic", 0.032f);

		ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		ShaderManager::getInstance()->getDeferredShading()->setUniform("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	//}

	//glm::mat4 viewMatrix = this->camara->getView();
	//glm::mat4 projectionMatrix = this->camara->getProjection();

	ShaderManager::getInstance()->getDeferredShading()->setUniform("viewPos", this->camara->getPosition());
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("viewInverse", glm::inverse(viewMatrix));
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("projectionInverse", glm::inverse(projectionMatrix));

	// Shadowmap code
	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DepthStencilTextureShadowMap);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("shadowmap", 0);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("lightSpaceViewProjectionMatrix", directionalLightViewProjMatrix);*/

	renderQuad();

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
}

void Scene::postProcessEffectsPass()
{
	//Gamma Correction!!

	//BLOOM

	//HDR

	//fxaa

	//chromaticAberration??
}

/* Recorre */
void Scene::DrawObjs()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Datos necesarios
	glm::mat4 mView = camara->getView();
	glm::mat4 mViewProjection = camara->getMatrixViewProjection();

	// 1. Shadow map
	// -----------------------------------------------------------------
	//shadowMapPass();

	// 2. geometry pass: render scene's geometry/color data into gbuffer
	// -----------------------------------------------------------------
	gBufferPass(mView, mViewProjection);

	// 3. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
	// -----------------------------------------------------------------------------------------------------------------------
	deferredLightPass();
	
	// 4. Forward rendering
	// ----------------------------------------------------------------------------------
	//for (int i = 0; i < NR_LIGHTS; i++) {
		//this->nodoLight->DrawObjs(mView, mViewProjection);
	//}

	// 5. Forward rendering
	// ----------------------------------------------------------------------------------
	//postProcessEffectsPass();
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