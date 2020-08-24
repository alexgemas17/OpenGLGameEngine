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

Application::Application(): 
	world(nullptr), wrappRaton(true), lastTime(glfwGetTime()),
	deltaTime(0.0f), lastFrame(0.0f), nbFrames(0)
{
	ExePath();
	this->guiManager = new GuiManager();
	this->textureManager = new TextureManager();
	this->world = new Scene();
}

Application::~Application()
{
	delete world;
	delete textureManager;
}

void Application::MainLoop(GLFWwindow* window)
{
	// ----------------------------- DELTA TIME ----------------------
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Comprobamos si debemos bloquear o no el ratón para el uso de la GUI.
	/*if (InputManager::getInstance()->getInputButtonDown(Key_F1)) {
		this->wrappRaton = !this->wrappRaton;
	}*/

	//--------------- GUI INIT -------------
	//guiManager->StartGUI();

	// ----------------------------- FPS ----------------------
	double currentTime = glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
		// printf and reset timer
		printf("---------------\n");
		printf("%f ms/frame\n", 1000.0 / double(nbFrames));
		printf("%f n/frame\n", 1 / (0.001 * (1000.0 / double(nbFrames))));
		nbFrames = 0;
		lastTime += 1.0;
	}

	// ----------------------------- INPUTS ----------------------
	//processInput(window);
	InputManager::getInstance()->key_callback(window);

	// ----------------------------- UPDATE ----------------------
	UpdateMainScene(deltaTime);

	// ----------------------------- RENDER ----------------------
	DrawMainScene();

	//--------------- GUI RENDER -------------
	//guiManager->showGUI(); TO-DO: ARREGLAR
}

void Application::InitMainScene()
{
	world = new Scene();
	world->InitCamara(FOV, WIDHT, HEIGHT, ZNEAR, ZFAR);
	world->InitScene();
}

void Application::InitTextures()
{
	this->textureManager->LoadTextures();
}

void Application::InitGUI() 
{
	//Añadimos los objetos a la gui.
	this->guiManager->addNodoSceneToGUI(world->getNodesScene(), "sponza");
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

	//pos - 9 --> Para eliminar de "x64\\Debug" de la ruta

	this->url_Path = std::string(buffer).substr(0, pos - 9);
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

GuiManager* Application::getGuiManager()
{
	return this->guiManager;
}