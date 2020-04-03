#include "Scene.h"

#include "../Input/InputManager.h"
#include "../Loaders/FileLoader.h"
#include "../Application.h"

#include <../glm/gtc/type_ptr.hpp>

Scene::Scene(): camara(nullptr)
{
	glPrimitiveRestartIndex(0xFFFFFFFF); //Posible incompatibilidad con los modelos cargados desde Assimp.
	glEnable(GL_PRIMITIVE_RESTART);
	glEnable(GL_PROGRAM_POINT_SIZE);

	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	//glEnable(GL_MULTISAMPLE);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	//glDepthFunc(GL_LESS);
	//glEnable(GL_BLEND);
	//glDisable(GL_BLEND); //Activar despues del Deferred rendering si se quiere usar
}

Scene::~Scene() {}

void Scene::InitScene()
{
	LoadObjs();
	InitLights();

	InitShadowMapBuffer();
	InitGBuffer();
	InitDeferredLightBuffer();

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

	delete loader;
}

float RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

/* Crea aleatoriamente un n�mero determinado de luces*/
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
		float xPos = RandomFloat(-10, 10);
		float yPos = RandomFloat(10, 20);
		float zPos = RandomFloat(-10, 10);
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
	// configure depth map FBO
	// -----------------------
	glGenFramebuffers(1, &shadowMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

	//  ---------------- Detph Stencil Data  ---------------- 
	glGenTextures(1, &DepthStencilTextureShadowMap);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DepthStencilTextureShadowMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, (const void*) nullptr);

	// Texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));

	// Texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Mipmapping --> No tiene??
	//glGenerateMipmap(GL_TEXTURE_2D);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, m_TextureSettings.MipBias);

	// Anisotropic filtering (TODO: Move the anistropyAmount calculation to Defs.h to avoid querying the OpenGL driver everytime)
	/*float maxAnisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	float anistropyAmount = glm::min(maxAnisotropy, 1.0f);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anistropyAmount);*/

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthStencilTextureShadowMap, 0);

	/*glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);*/

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error, ShadowBuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/* Crear el buffer donde se guardar�n los datos que se pasar� posteriormente a la pasada de luces*/
void Scene::InitGBuffer()
{
	// configure g-buffer framebuffer
	// ------------------------------
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	//  ---------------- Detph Stencil Data  ---------------- 
	glGenTextures(1, &DepthStencilTextureGBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, DepthStencilTextureGBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, (const void*) nullptr);

	// Texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));

	// Texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Mipmapping --> No tiene??
	//glGenerateMipmap(GL_TEXTURE_2D);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, m_TextureSettings.MipBias);

	// Anisotropic filtering (TODO: Move the anistropyAmount calculation to Defs.h to avoid querying the OpenGL driver everytime)
	/*float maxAnisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	float anistropyAmount = glm::min(maxAnisotropy, 1.0f);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anistropyAmount);*/

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, DepthStencilTextureGBuffer, 0);
		
	// ---------------- GBuffer Data  ---------------- 
	// Texture data info
	glGenTextures(1, &gb_Albedo);
	glBindTexture(GL_TEXTURE_2D, gb_Albedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_RGB, GL_UNSIGNED_BYTE, (const void*) nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Filtro anisotropico
	/*float maxAnisotropy1;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy1);
	float anistropyAmount1 = glm::min(maxAnisotropy1, 1.0f);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anistropyAmount1);*/

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gb_Albedo, 0);

	// Normal texture data
	glGenTextures(1, &gb_Normal);
	glBindTexture(GL_TEXTURE_2D, gb_Normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_RGB, GL_UNSIGNED_BYTE, (const void*) nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Filtro anisotropico
	/*float maxAnisotropy2;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy2);
	float anistropyAmount2 = glm::min(maxAnisotropy, 1.0f);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anistropyAmount2);*/

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gb_Normal, 0);

	// Material info: metallic, roughness, ao
	glGenTextures(1, &gb_MaterialInfo);
	glBindTexture(GL_TEXTURE_2D, gb_MaterialInfo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_RGB, GL_UNSIGNED_BYTE, (const void*) nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Filtro anisotropico
	/*float maxAnisotropy3;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy3);
	float anistropyAmount3 = glm::min(maxAnisotropy, 1.0f);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anistropyAmount3);*/
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gb_MaterialInfo, 0);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);

	// create and attach depth buffer (renderbuffer)
	//unsigned int rboDepth;
	//glGenRenderbuffers(1, &rboDepth);
	//glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT());
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error, GBuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::InitDeferredLightBuffer()
{
	glGenFramebuffers(1, &lightBuffer);

	//  ---------------- Color texture  ---------------- 
	glBindFramebuffer(GL_FRAMEBUFFER, lightBuffer);
	glGenTextures(1, &ColorTextureDeferredLight);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ColorTextureDeferredLight);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_RGB, GL_UNSIGNED_BYTE, (const void*) nullptr);

	// Texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Mipmapping --> No tiene??
	//glGenerateMipmap(GL_TEXTURE_2D);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, m_TextureSettings.MipBias);

	// Anisotropic filtering (TODO: Move the anistropyAmount calculation to Defs.h to avoid querying the OpenGL driver everytime)
	/*float maxAnisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	float anistropyAmount = glm::min(maxAnisotropy, 1.0f);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anistropyAmount);*/

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTextureDeferredLight, 0);


	//  ---------------- Detph Stencil Data  ---------------- 
	glBindFramebuffer(GL_FRAMEBUFFER, lightBuffer);
	glGenTextures(1, &DepthStencilTextureDeferredLight);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, DepthStencilTextureDeferredLight);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, (const void*) nullptr);

	// Texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Mipmapping --> No tiene??
	//glGenerateMipmap(GL_TEXTURE_2D);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, m_TextureSettings.MipBias);

	// Anisotropic filtering (TODO: Move the anistropyAmount calculation to Defs.h to avoid querying the OpenGL driver everytime)
	/*float maxAnisotropy1;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy1);
	float anistropyAmount1 = glm::min(maxAnisotropy1, 1.0f);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy1);*/

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, DepthStencilTextureDeferredLight, 0);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, attachments);

	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);

	//finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error, DeferredLightBuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/* Inicializa la c�mara */
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
unsigned int quadIBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f
		};

		unsigned int index[] = { 1,0,2,3,1,2 };

		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindVertexArray(quadIBO);

		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &index[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Scene::shadowMapPass()
{
	// 1. render depth of scene to texture (from light's perspective)
	// --------------------------------------------------------------

	//lightSpaceMatrix = lightProjection * lightView
	glm::mat4 lightSpaceMatrix = this->camara->getProjection() * this->camara->getView();

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderManager::getInstance()->getShadowMap()->use();

	//LUZ DIRECCIONAL
	glm::vec3 dirLightShadowmapLookAtPos = this->camara->getPosition() + (glm::normalize(this->camara->getFront()) * 50.0f);
	glm::vec3 dirLightShadowmapEyePos = dirLightShadowmapLookAtPos + (-glm::vec3(-0.25f, -1.0f, -0.25f) * 100.0f);
	glm::mat4 directionalLightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 400.0f);
	glm::mat4 directionalLightView = glm::lookAt(dirLightShadowmapEyePos, dirLightShadowmapLookAtPos, glm::vec3(0.0f, 1.0f, 0.0f));
	directionalLightViewProjMatrix = directionalLightProjection * directionalLightView;
	ShaderManager::getInstance()->getShadowMap()->setUniform("lightSpaceViewProjectionMatrix", directionalLightViewProjMatrix);

	// Render models
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	//PASAR UNIFORM MODELMATRIX 
	this->nodoWorld->DrawObjsShadowMap();

	// Render terrain
	//this->nodoTerreno->DrawObjs(mView, mViewProjection);

	// reset viewport
	glViewport(0, 0, WIDHT, HEIGHT);
}

void Scene::gBufferPass(glm::mat4& mView, glm::mat4& mViewProjection)
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_MULTISAMPLE);
	
	// Setup Stencil mode
	/*glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMaskSeparate(GL_FRONT_AND_BACK, 0x00);
	glEnable(GL_STENCIL_TEST)*/;

	//Pasamos los datos importantes al GBufferShader
	ShaderManager::getInstance()->getGBuffer()->use();
	ShaderManager::getInstance()->getGBuffer()->setUniform("VPMatrix", this->camara->getMatrixViewProjection());

	// Draw de objetos
	//glStencilMaskSeparate(GL_FRONT_AND_BACK, 0xFF);
	//glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_ALWAYS, 0x01, 0xFF); // 0x01 -> Un modelo, 0x02 -> otro modelo, ..... 
	//glCullFace(GL_BACK);

	this->nodoWorld->DrawObjs(ShaderManager::getInstance()->getGBuffer());

	/*glStencilMaskSeparate(GL_FRONT_AND_BACK, 0x00);*/

	//Lo mismo que lo anterior pero para el terreno??
	//�Ser�a necesario?
	//TO-DO: Ver si con solo cambiar el shader podemos hacer menos guarro el c�digo

	/*glDisable(GL_STENCIL_TEST);*/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::deferredLightPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, lightBuffer);
	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_MULTISAMPLE);

	// Move the depth + stencil of the GBuffer to the our framebuffer
	// NOTE: Framebuffers have to have identical depth + stencil formats for this to work
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lightBuffer);
	glBlitFramebuffer(
		0, 
		0, 
		Application::getInstance()->getWIDHT(), 
		Application::getInstance()->getHEIGHT(),
		0,
		0,
		Application::getInstance()->getWIDHT(),
		Application::getInstance()->getHEIGHT(), 
		GL_DEPTH_BUFFER_BIT , GL_NEAREST
		);

	// Setup initial stencil state
	//glEnable(GL_STENCIL_TEST);
	//glStencilMaskSeparate(GL_FRONT_AND_BACK, 0x00); // Do not update stencil values

	//Pasamos los datos importantes al DeferredLightPassShader and bind GBuffer data
	ShaderManager::getInstance()->getDeferredShading()->use();

	// ------------------------ Light Pass ------------------------
	//NOTA: Las luces est�n dentro del shader. TO-DO: Ver como pasarla aqu� para ir de una en una.
	ShaderManager::getInstance()->getDeferredShading()->setUniform("numDirLights", NR_DIRECTIONAL_LIGHTS);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("numPointLights", NR_POINT_LIGHTS);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("numtSpotLights", NR_SPOT_LIGHTS);

	for (int i = 0; i < NR_DIRECTIONAL_LIGHTS; i++) {	//Solo hay 1
		ShaderManager::getInstance()->getDeferredShading()->setUniform(("dirLights[" + std::to_string(i) + "].direction").c_str(), glm::vec3(-0.25f, -1.0f, -0.25f));
		ShaderManager::getInstance()->getDeferredShading()->setUniform(("dirLights[" + std::to_string(i) + "].intensity").c_str(), 2.0f);
		ShaderManager::getInstance()->getDeferredShading()->setUniform(("dirLights[" + std::to_string(i) + "].lightColour").c_str(), glm::vec3(3.25f, 3.25f, 3.25f));
	}

	for (int i = 0; i < NR_POINT_LIGHTS; i++) {
		ShaderManager::getInstance()->getDeferredShading()->setUniform(("pointLights[" + std::to_string(i) + "].position").c_str(), lightPositions[i]);
		ShaderManager::getInstance()->getDeferredShading()->setUniform(("pointLights[" + std::to_string(i) + "].intensity").c_str(), 10.0f);
		ShaderManager::getInstance()->getDeferredShading()->setUniform(("pointLights[" + std::to_string(i) + "].lightColour").c_str(), lightColors[i]);
		ShaderManager::getInstance()->getDeferredShading()->setUniform(("pointLights[" + std::to_string(i) + "].attenuationRadius").c_str(), 30.0f);
	}

	for (int i = 0; i < NR_SPOT_LIGHTS; i++) {		//Solo hay 1
		ShaderManager::getInstance()->getDeferredShading()->setUniform(("spotLights[" + std::to_string(i) + "].position").c_str(), glm::vec3(0.0f, 3.0f, 0.0f));
		ShaderManager::getInstance()->getDeferredShading()->setUniform(("spotLights[" + std::to_string(i) + "].direction").c_str(), glm::vec3(0.0f, -1.0f, 0.0f));
		ShaderManager::getInstance()->getDeferredShading()->setUniform(("spotLights[" + std::to_string(i) + "].intensity").c_str(), 100.0f);
		ShaderManager::getInstance()->getDeferredShading()->setUniform(("spotLights[" + std::to_string(i) + "].lightColour").c_str(), glm::vec3(1.0f, 1.0f, 1.0f));
		ShaderManager::getInstance()->getDeferredShading()->setUniform(("spotLights[" + std::to_string(i) + "].attenuationRadius").c_str(), 50.0f);
		ShaderManager::getInstance()->getDeferredShading()->setUniform(("spotLights[" + std::to_string(i) + "].cutOff").c_str(), glm::cos(glm::radians(12.5f)));
		ShaderManager::getInstance()->getDeferredShading()->setUniform(("spotLights[" + std::to_string(i) + "].outerCutOff").c_str(), glm::cos(glm::radians(15.0f)));
	}

	glm::mat4 viewMatrix = this->camara->getView();
	glm::mat4 projectionMatrix = this->camara->getProjection();

	ShaderManager::getInstance()->getDeferredShading()->setUniform("viewPos", this->camara->getPosition());
	ShaderManager::getInstance()->getDeferredShading()->setUniform("viewInverse", glm::inverse(viewMatrix));
	ShaderManager::getInstance()->getDeferredShading()->setUniform("projectionInverse", glm::inverse(projectionMatrix));

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gb_Albedo);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("albedoTexture", 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, gb_Normal);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("normalTexture", 5);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, gb_MaterialInfo);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("materialInfoTexture", 6);

	//glActiveTexture(GL_TEXTURE7);
	//preLightingOutput.ssaoTexture->bind(7);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("ssaoTexture", 7);

	glActiveTexture(GL_TEXTURE8); 
	glBindTexture(GL_TEXTURE_2D, DepthStencilTextureGBuffer);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("depthTexture", 8);

	//ShaderManager::getInstance()->getDeferredShading()->setUniform("nearPlane", NEAR_PLANE);
	//ShaderManager::getInstance()->getDeferredShading()->setUniform("farPlane", FAR_PLANE);

	// Shadowmap code
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DepthStencilTextureShadowMap);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("shadowmap", 0);
	ShaderManager::getInstance()->getDeferredShading()->setUniform("lightSpaceViewProjectionMatrix", directionalLightViewProjMatrix);

	// Draw terrain light??????

	//glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_EQUAL, 0x01, 0xFF); // Hacerlo para cada modelo: 0x01, 0x02, 0x03...
	renderQuad(); //Plane

	// Reset state
	glEnable(GL_DEPTH_TEST);
	/*glDisable(GL_STENCIL_TEST);*/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::postProcessEffectsPass()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, lightBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(
		0,
		0,
		Application::getInstance()->getWIDHT(),
		Application::getInstance()->getHEIGHT(),
		0,
		0,
		Application::getInstance()->getWIDHT(),
		Application::getInstance()->getHEIGHT(),
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	);

	//Gamma Correction!!

	//BLOOM

	//HDR

	//fxaa

	//chromaticAberration??

	// Finally render the scene to the window's framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	ShaderManager::getInstance()->getCopyDataPass()->use();
	ShaderManager::getInstance()->getCopyDataPass()->setUniform("input_texture", 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ColorTextureDeferredLight);
	renderQuad();
}

void Scene::restoreDefaultDepthBuffer()
{
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
	//// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
	//// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
	//// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
	//glBlitFramebuffer(0,
	//	0,
	//	Application::getInstance()->getWIDHT(), Application::getInstance()->getHEIGHT(),
	//	0,
	//	0, Application::getInstance()->getWIDHT(),
	//	Application::getInstance()->getHEIGHT(),
	//	GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/* Recorre */
void Scene::DrawObjs()
{
	//Datos necesarios
	glm::mat4 mView = camara->getView();
	glm::mat4 mViewProjection = camara->getMatrixViewProjection();

	// 1. Shadow map
	// -----------------------------------------------------------------
	shadowMapPass();

	// 2. geometry pass: render scene's geometry/color data into gbuffer
	// -----------------------------------------------------------------
	gBufferPass(mView, mViewProjection);

	// 3. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
	// -----------------------------------------------------------------------------------------------------------------------
	deferredLightPass();

	// �Es necesario? Porque estamos usando diferentes buffer que luego vamos a combinar.
	// 3.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
	// ----------------------------------------------------------------------------------
	//restoreDefaultDepthBuffer();
	
	// 4. Forward rendering
	// ----------------------------------------------------------------------------------
	//for (int i = 0; i < NR_LIGHTS; i++) {
		//this->nodoLight->DrawObjs(mView, mViewProjection);
	//}

	// 5. Forward rendering
	// ----------------------------------------------------------------------------------
	postProcessEffectsPass();
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