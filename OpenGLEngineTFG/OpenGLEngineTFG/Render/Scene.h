#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "../Loaders/AssimpLoader.h"
#include "../Camara/Camara.h"
#include "SceneObj.h"
#include "NodoScene.h"
#include "Cubemap/CubeMap.h"

#include "ForwardRender.h"
#include "DeferredShadingRender.h"
#include "ForwardPlusRender.h"


//---------- INCLUDE PRUEBAS --------------
#include "Render.h"

const int NR_DIRECTIONAL_LIGHTS = 1;
const int NR_POINT_LIGHTS = 150;
const int NR_SPOT_LIGHTS = 1;
const unsigned int SHADOW_WIDTH = 2048 * 2, SHADOW_HEIGHT = 2048 * 2;
const float NEAR_PLANE = 0.01f, FAR_PLANE = 1000.0f;

class Scene
{
public:
	Scene();
	~Scene();

	void InitScene();
	void InitCamara(float fov, int width, int height, float zNear, float zFar);
	void UpdateObjs(float deltaTime);
	void DrawObjs();

	Camara* camara;

	/* Funciones callbacks */
	void framebuffer_size_callback(int width, int height); 
	void mouse_button_callback(int button, int action, int mods);
	void cursor_position_callback(double xpos, double ypos);
	void scroll_callback(double xoffset, double yoffset);

	NodoScene* getNodesScene() { return this->nodoWorld; }

	/* Shadow map data*/
	glm::mat4 lightSpaceMatrix;

	std::vector<SceneObj*> objs;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	std::vector<float> lightIntensity;

private:
	std::vector<NodoScene*> objetosScena; //Nota: Para cuando carge de texto
	NodoScene* nodoWorld; 
	NodoScene* nodoLight;
	glm::vec3 lightPosition;

	CubeMap* skybox;

	unsigned int shadowMap, DepthShadowMap;
	unsigned int ssaoFBO, ssaoBlurFBO, ssaoColorBuffer, ssaoColorBufferBlur;

	int mode;
	ForwardRender* forwardRender;
	DeferredShadingRender* deferredShadingRender;
	ForwardPlusRender* forwardPlusRender;

	/* ShadowMap */
	glm::mat4 shadowBias;

	/* SSAO and BLUR dada*/
	unsigned int noiseTexture;
	std::vector<glm::vec3> ssaoKernel; 
	std::vector<glm::vec3> ssaoNoise;

	/* Screen Data */
	int SCR_WIDTH;
	int SCR_HEIGHT;

	// -------- PRIVATE FUNC ------------
	void LoadObjs();	// Carga desde el objs.txt los objetos que tiene la escena
	void InitLights();	// Crea luces puntuales aleatoriamente (hardcode: spot y direccional)

	// Inits de los buffers correspondientes.
	void InitShadowMapBuffer();
	void InitSSAOBuffer();

	// Pasadas de las luces.
	void shadowMapPass();
	void ssaoPass(glm::mat4& mView, glm::mat4& mProj);
	void skyboxRender(glm::mat4 mView, glm::mat4 mProj);
	void postProcessEffectsPass(glm::mat4& mViewProjection);
};

