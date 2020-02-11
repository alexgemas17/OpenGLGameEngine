#include "Application.h"

#include <windows.h> // Para obtener el path de la app.

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

Application::Application(): world(nullptr), url_path("")
{
	ExePath();
}

Application::~Application()
{
	mainObjs.clear();
	delete world;
}

// ------------------------------ PUBLIC ------------------------------ 
void Application::InitMainScene()
{
	world = new Scene();
	world->InitCamara(FOV, WIDHT, HEIGHT, ZNEAR, ZFAR);
	world->InitObjs();
}

/*
	Llama a la función update del nodo principal de la escena para que los objetos, los que los requieran, 
	pueda comprar los inputs y demas.
*/
void Application::UpdateMainScene(float deltaTime)
{
	world->UpdateObjs(deltaTime);
}

/*
	Llama a la función del nodo principal para que dibuje todos los objetos que contienen la escena.
*/
void Application::DrawMainScene()
{
	world->DrawObjs();
}

/*
	Obtiene la información de la gráfica en la que se está trabajando: modelo, vendedor, versión de OpenGL y versión del HLSL
*/
void Application::getInfoHardware()
{
	//  -------------------- Info de la gráfica  -------------------- 
	std::cout << glGetString(GL_RENDERER) << std::endl;
	std::cout << glGetString(GL_VENDOR) << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

/*
	Devuelve la url de la path donde se está ejecutando la app.
*/
std::string Application::getPathApp()
{
	return this->url_path;
}

/*
	Devuelve el nodo de la escena principal.
*/
Scene* Application::getMainScene()
{
	return this->world;
}

// ------------------------------ PRIVATE ------------------------------
/*
	Lee, desde el fichero Data\\Objs.txt, los objetos que componen la escena.
*/
void Application::LoadObjsFromFile()
{
	//Leemos los objetos que necesitamos del archivo.
	//TODO
}

/*
	Obtiene del sistema la path desde donde se está ejecutando la app.
*/
void Application::ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");

	//pos - 5 --> Para quitar Debug de la ruta.
	this->url_path = std::string(buffer).substr(0, pos - 5);
}