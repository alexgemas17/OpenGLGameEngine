#include <iostream>
#include <cstdlib>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm.hpp>

#include "Input/InputManager.h"
#include "Managers/ShaderManager.h"
#include "Application.h"

void showFPSCounter();
void setFunctionsCallbacks(GLFWwindow* window);
void window_refresh_callback(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

int main() {

	//  -------------------- Iniciamos GLFW -------------------- 
	if (glfwInit() != GLFW_TRUE) {
		std::cout << "ERROR al inicializar GLFW" << std::endl;
		return -1;
	}

	//  -------------------- Creamos la ventana de la app -------------------- 
	GLFWwindow* window = glfwCreateWindow(WIDHT, HEIGHT, "TFG UJA ENGINE", nullptr, nullptr);
	glfwSetWindowPos(window, 300, 150);

	if (window == nullptr) {
		std::cout << "ERROR al abrir la ventada de GLFW" << std::endl;
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(window);

	//  -------------------- Configuramos GLFW -------------------- 
	glfwWindowHint(GLFW_SAMPLES, 4); // Activa antialiasing con 4 muestras.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwSwapInterval(0); //VSync off

	//  -------------------- Iniciamos GLEW -------------------- 
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return -3;
	}

	glViewport(0, 0, WIDHT, HEIGHT);

	// -------------------- Registramos los callbacks -------------------- 
	setFunctionsCallbacks(window);

	// -------------------- Creamos los shaders -------------------- 
	ShaderManager::getInstance();

	// -------------------- Creamos la escena --------------------  
	Application::getInstance()->InitMainScene();

	//Para el FPS Counter
	/*double lastTime = glfwGetTime();
	int nbFrames = 0;*/

	// Para poder establecer el DeltaTime
	float deltaTime = 0.0f;	// Tiempo entre dos frames
	float lastFrame = 0.0f;
	// -------------------- RENDER LOOP --------------------  
	while (!glfwWindowShouldClose(window)) {

		// ----------------------------- DELTA TIME ----------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		// ----------------------------- INPUTS ----------------------
		//processInput(window);
		InputManager::getInstance()->key_callback(window);

		// ----------------------------- UPDATE ----------------------
		Application::getInstance()->UpdateMainScene(deltaTime);
		
		// ----------------------------- RENDER ----------------------
		Application::getInstance()->DrawMainScene();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Liberamos recursos
	Application::getInstance()->DestroyInstance();
}

// --------------------------- FUNCIONES ----------------------------
void showFPSCounter() 
{
	// ----------------------------- FPS ----------------------
		//double currentTime = glfwGetTime();
		//nbFrames++;
		//if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
		//	// printf and reset timer
		//	printf("---------------\n");
		//	printf("%f ms/frame\n", 1000.0 / double(nbFrames));
		//	printf("%f n/frame\n", 1 / (0.001 * (1000.0 / double(nbFrames))));
		//	nbFrames = 0;
		//	lastTime += 1.0;
		//}
}

void setFunctionsCallbacks(GLFWwindow* window)
{
	glfwSetWindowRefreshCallback(window, window_refresh_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetKeyCallback(window, key_callback); --> Se hace en el render loop
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	//Hace que mientras se pulse una tecla, ésta devolverá GL_PRESS todo el rato.
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
}

void window_refresh_callback(GLFWwindow* window) {
	//Llamamos a la función correspodiente a las acciones al refrescar la ventana.
	Application::getInstance()->DrawMainScene();

	glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	Application::getInstance()->getMainScene()->framebuffer_size_callback(width, height);

	window_refresh_callback(window);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		//PagRenderer::getInstance()->ratonPress(button);
	}
	else if (action == GLFW_RELEASE) {
		//PagRenderer::getInstance()->ratonRelease(button);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	Application::getInstance()->getMainScene()->scroll_callback(xoffset, yoffset);
	window_refresh_callback(window);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	//Hacemos que el cursor se quede bloqueado en medio y desaparezca.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Application::getInstance()->getMainScene()->cursor_position_callback(xpos, ypos);

	window_refresh_callback(window);
}