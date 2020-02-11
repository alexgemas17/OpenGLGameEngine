#pragma once

#include <glm.hpp>
#include <string>
#include <vector>
#include "Render/Scene.h"
#include "Loaders/FileLoader.h"
#include "TextureManager.h"

// --------------- Constantes variables --------------- 
const float FOV = 45.0f;
const int   WIDHT = 1024;
const int   HEIGHT = 576;
const float ZNEAR = 0.1f;
const float ZFAR = 100.0f;


// --------------- Constantes rutas --------------- 
const std::string url_Config = "Data\\Config.txt";
const std::string url_ObjsInfo = "Data\\Objs.txt";
const std::string url_Models = "Data\\3DModels\\asdf.asdf";


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
	std::string getPathApp();
	Scene* getMainScene();

	static Application* getInstance();
	void DestroyInstance();

private:
	std::string url_path;
	TextureManager textureManager;
	Scene* world;
	std::vector<ObjFile> mainObjs;

	void LoadObjsFromFile();
	void ExePath();

	// Instancia singleton.
	static Application* instance;
};

