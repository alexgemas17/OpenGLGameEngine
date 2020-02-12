#pragma once

#include <glm.hpp>
#include <string>
#include <vector>
#include "Render/Scene.h"
#include "TextureManager.h"

const float FOV = 45.0f;
const int WIDHT = 1024;
const int HEIGHT = 576;
const float ZNEAR = 0.1f;
const float ZFAR = 100.0f;

const std::string url_Config = "";
const std::string url_Objs = "";
const std::string url_Models = "";

/**
	Clase que se encargará de gestionar todo sobre el programa.
*/
class Application
{
public:
	Application();
	~Application();

	void InitMainScene();
	void InitTextures();
	void UpdateMainScene(float deltaTime);
	void DrawMainScene();

	std::string getPath();
	void getInfoHardware();
	Scene* getMainScene();
	TextureManager* getTextureManager();

	static Application* getInstance();
	void DestroyInstance();

private:
	std::string url_Path;
	Scene* world;
	TextureManager* textureManager;

	void ExePath();

	// Instancia singleton.
	static Application* instance;
};

