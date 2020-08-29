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

#include "ForwardRender.h"
#include "DeferredShadingRender.h"
#include "ForwardPlusRender.h"


//---------- INCLUDE PRUEBAS --------------
#include "Render.h"

const int MAX_LIGHTS = 2500;
const int MAX_TRANSP_OBJS = 500;
const float NEAR_PLANE = 0.01f, FAR_PLANE = 1000.0f;

class Scene
{
public:
	Scene();
	~Scene();

	void InitScene();
	void InitCamara(float fov, int width, int height, float zNear, float zFar);
	void AddNewNumLights(int numLights);
	void UpdateObjs(float deltaTime);
	void SetUniforms();
	void DrawObjs();

	NodoScene* getNodesScene() { return this->nodoWorld; }
	unsigned int getSceneFBO() { return this->sceneFBO; }
	unsigned int getSceneColorBuffer() { return this->sceneColorBuffer; }
	void setMode(int mode);

	/* Funciones callbacks */
	void framebuffer_size_callback(int width, int height); 
	void cursor_position_callback(double xpos, double ypos);

	/* Datos de las luces */
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	std::vector<float> lightIntensity;

	/* Datos de los objetos transparentes */
	std::vector<glm::vec3> transparentObjPositions;

	/* Cámara */
	Camara* camara;

	/* Renders */
	ForwardRender* forwardRender;
	DeferredShadingRender* deferredShadingRender;
	ForwardPlusRender* forwardPlusRender;

	std::vector<SceneObj*> objs;

	int NUM_LIGHTS;
	int NUM_TRANSP_OBJS;
	float exposure;
	float gamma;

private:
	int mode;
	unsigned int sceneFBO, sceneColorBuffer;

	NodoScene* nodoWorld;
	NodoScene* transparentObj;

	CubeMap* skybox;
	Cube* cube;

	/* Screen Data */
	int SCR_WIDTH;
	int SCR_HEIGHT;

	// -------- PRIVATE FUNC ------------
	void LoadObjs();	// Carga desde el objs.txt los objetos que tiene la escena
	void InitLights();	// Crea luces puntuales aleatoriamente
	void UpdateLights(float deltaTime);

	// Pasadas de las luces.
	void skyboxRender();
	void postProcessEffectsPass(glm::mat4& mViewProjection);
};

