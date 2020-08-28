#pragma once

#include <glm.hpp>
#include <string>
#include <vector>

#include "Render/Scene.h"
#include "Managers/TextureManager.h"
#include "Managers/GuiManager.h"

const float FOV = 45.0f;
const int WIDHT = 1366;
const int HEIGHT = 768;
const float ZNEAR = 0.1f;
const float ZFAR = 1000.0f;

const std::string url_Config = "Data\\Config.txt";
const std::string url_Objs = "Data\\Lights.txt";
const std::string url_Models = "Data\\3DModels\\";

const std::string no_texture = "Data\\Textures\\no-texture.png";

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
	void InitGUI(); 
	void SetUniforms();

	void MainLoop(GLFWwindow* window);

	bool wrappRaton;

	std::string getPath();
	void getInfoHardware();
	Scene* getMainScene();
	TextureManager* getTextureManager();
	GuiManager* getGuiManager();

	int getWIDHT() { return WIDHT; }
	int getHEIGHT() { return HEIGHT; }
	bool getWrappRaton() { return this->wrappRaton; }

	static Application* getInstance();
	void DestroyInstance();

private:
	std::string url_Path;
	Scene* world;
	TextureManager* textureManager;
	GuiManager* guiManager;

	// Tiempo entre dos frames
	float deltaTime; 
	float lastFrame;

	// Datos para el FPS Counter
	double lastTime;
	int nbFrames;

	// -------------------- FUNCIONES PRIVADAS --------------------  
	void ExePath(); //Obtienes el path de la aplicación.
	void UpdateMainScene(float deltaTime);
	void DrawMainScene();

	// Instancia singleton.
	static Application* instance;
};