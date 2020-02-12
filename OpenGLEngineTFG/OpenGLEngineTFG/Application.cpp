#include "Application.h"

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

Application::Application(): world(nullptr) {}

Application::~Application()
{
	mainObjs.clear();
	delete world;
}

void Application::getInfoHardware()
{
	//  -------------------- Info de la gráfica  -------------------- 
	std::cout << glGetString(GL_RENDERER) << std::endl;
	std::cout << glGetString(GL_VENDOR) << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

Scene* Application::getMainScene()
{
	return this->world;
}

void Application::InitMainScene()
{
	world = new Scene();
	world->InitCamara(FOV, WIDHT, HEIGHT, ZNEAR, ZFAR);
	world->InitObjs();
}

void Application::UpdateMainScene(float deltaTime)
{
	world->UpdateObjs(deltaTime);
}

void Application::DrawMainScene()
{
	world->DrawObjs();
}