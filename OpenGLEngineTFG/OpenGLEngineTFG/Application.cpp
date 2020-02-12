#include "Application.h"

#include <Windows.h>

Application* Application::instance = nullptr;

// Acceder al singleton.
Application* Application::getInstance()
{
	if (!instance) {
		instance = new Application();
	}
	return instance;
}

void Application::DestroyInstance() {
	delete instance;
	instance = 0;
}

Application::Application(): world(nullptr)
{
	ExePath();
	this->textureManager = new TextureManager();
	this->world = new Scene();
}

Application::~Application()
{
	delete world;
	delete textureManager;
}

void Application::getInfoHardware()
{
	//  -------------------- Info de la gráfica  -------------------- 
	std::cout << glGetString(GL_RENDERER) << std::endl;
	std::cout << glGetString(GL_VENDOR) << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

std::string Application::getPath()
{
	return this->url_Path;
}

Scene* Application::getMainScene()
{
	return this->world;
}

TextureManager* Application::getTextureManager()
{
	return this->textureManager;
}

void Application::InitMainScene()
{
	world = new Scene();
	world->InitCamara(FOV, WIDHT, HEIGHT, ZNEAR, ZFAR);
	world->InitObjs();
}

void Application::InitTextures()
{
	this->textureManager->LoadTextures();
}

void Application::UpdateMainScene(float deltaTime)
{
	world->UpdateObjs(deltaTime);
}

void Application::DrawMainScene()
{
	world->DrawObjs();
}

void Application::ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");

	//pos - 5 --> Para quitar Debug de la ruta.
	this->url_Path = std::string(buffer).substr(0, pos - 5);
}
