#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "../Loaders/AssimpLoader.h"
#include "../Camara/Camara.h"
#include "SceneObj.h"
#include "NodoScene.h"
#include "Cubemap/CubeMap.h"

#include "../BasicElement/Cube.h"

//---------- INCLUDE PRUEBAS --------------
#include "Render.h"

const int NR_DIRECTIONAL_LIGHTS = 1;
const int NR_POINT_LIGHTS = 5;
const int NR_SPOT_LIGHTS = 1;
const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
const float NEAR_PLANE = 0.01f, FAR_PLANE = 10000.0f;

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

private:
	std::vector<NodoScene*> objetosScena; //Nota: Para cuando carge de texto
	NodoScene* nodoWorld; 
	NodoScene* nodoLight;
	glm::vec3 lightPosition;

	CubeMap* skybox;

	unsigned int gPosition, gNormal, gAlbedo, gMaterialInfo;
	unsigned int shadowMap, DepthShadowMap;
	unsigned int gBuffer, DepthGBuffer;
	unsigned int lightBuffer;

	/*TESTING*/
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;

	/* Shadow map data*/
	glm::mat4 lightSpaceMatrix;

	// -------- PRIVATE FUNC ------------
	void LoadObjs();	// Carga desde el objs.txt los objetos que tiene la escena
	void InitLights();	// Crea luces puntuales aleatoriamente (hardcode: spot y direccional)

	// Inits de los buffers correspondientes.
	void InitShadowMapBuffer();
	void InitGBuffer();
	void InitDeferredLightBuffer();

	// Pasadas de las luces.
	void shadowMapPass();
	void gBufferPass(glm::mat4 &mView, glm::mat4 & mViewProjection);
	void deferredLightPass();
	void forwardPass(glm::mat4 mView, glm::mat4 mProj);
	void postProcessEffectsPass();
};

