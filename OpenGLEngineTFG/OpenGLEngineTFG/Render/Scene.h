#pragma once

#include <vector>
#include <glm.hpp>
#include <GL\glew.h>

#include "../Loaders/AssimpLoader.h"
#include "../Camara/Camara.h"
#include "SceneObj.h"
#include "NodoScene.h"

//---------- INCLUDE PRUEBAS --------------
#include "Render.h"

class Scene
{
public:
	Scene();
	~Scene();

	void InitObjs();
	void InitCamara(float fov, int width, int height, float zNear, float zFar);
	void UpdateObjs(float deltaTime);
	void DrawObjs();

	/* Funciones callbacks */
	void framebuffer_size_callback(int width, int height); 
	void mouse_button_callback(int button, int action, int mods);
	void cursor_position_callback(double xpos, double ypos);
	void scroll_callback(double xoffset, double yoffset);

private:
	std::vector<NodoScene*> objetosScena; //Nota: Para cuando carge de texto
	NodoScene* nodo;

	Camara* camara;
};

