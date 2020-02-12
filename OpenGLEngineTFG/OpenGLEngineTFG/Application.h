#pragma once

#include <glm.hpp>
#include <string>
#include <vector>
#include "Render/Scene.h"
#include "Loaders/FileLoader.h"

const float FOV = 45.0f;
const int WIDHT = 1024;
const int HEIGHT = 576;
const float ZNEAR = 0.1f;
const float ZFAR = 100.0f;

/**
	Clase que se encargará de gestionar todo sobre el programa.
*/
class Application
{
public:
	Application();
	~Application();

	void InitMainScene();
	void UpdateMainScene(float deltaTime);
	void DrawMainScene();

	void getInfoHardware();
	Scene* getMainScene();

	static Application* getInstance();
	void DestroyInstance();

private:

	//TextureHandle textureHandle;
	Scene* world;
	std::vector<ObjFile> mainObjs;

	void LoadObjsFromFile();

	// Instancia singleton.
	static Application* instance;
};

