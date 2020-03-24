#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "../Loaders/AssimpLoader.h"
#include "../Camara/Camara.h"
#include "SceneObj.h"
#include "NodoScene.h"

#include "../BasicElement/Cube.h"

//---------- INCLUDE PRUEBAS --------------
#include "Render.h"

const unsigned int NR_LIGHTS = 32;

class Scene
{
public:
	Scene();
	~Scene();

	void InitScene();
	void InitCamara(float fov, int width, int height, float zNear, float zFar);
	void UpdateObjs(float deltaTime);
	void DrawObjs();

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

	Camara* camara;

	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gAlbedoSpec, gNormalSpec;
	unsigned int gTvector, gNvector, gBvector;


	/*TESTING*/
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;

	// -------- PRIVATE FUNC ------------
	void LoadObjs();
	void InitGBuffer();
	void InitLights();
};

