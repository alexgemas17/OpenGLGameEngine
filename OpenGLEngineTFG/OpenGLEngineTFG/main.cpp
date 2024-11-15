#include <iostream>
#include <cstdlib>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm.hpp>

#include "Gui/imgui.h"
#include "Gui/imgui_impl_glfw.h"
#include "Gui/imgui_impl_opengl3.h"

#include "Input/InputManager.h"
#include "Managers/ShaderManager.h"
#include "Application.h"

void setFunctionsCallbacks(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

float FloatAleatorio(float a, float b);

int main() {

	//  -------------------- Iniciamos GLFW -------------------- 
	if (glfwInit() != GLFW_TRUE) {
		std::cout << "ERROR al inicializar GLFW" << std::endl;
		return -1;
	}

	//  -------------------- Creamos la ventana de la app -------------------- 
	GLFWwindow* window = glfwCreateWindow(WIDHT, HEIGHT, "TFG UJA ENGINE", nullptr, nullptr);
	//glfwSetWindowPos(window, 300, 150);

	if (window == nullptr) {
		std::cout << "ERROR al abrir la ventada de GLFW" << std::endl;
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(window);

	//  -------------------- Configuramos GLFW -------------------- 
	glfwWindowHint(GLFW_SAMPLES, 4); // Activa antialiasing con 4 muestras.
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwSwapInterval(0); //VSync off
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//  -------------------- Iniciamos GLEW -------------------- 
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return -3;
	}

	// Make sure OpenGL version 4.5 API is available
	if (!GLEW_VERSION_4_5) {
		throw std::runtime_error("OpenGL 4.5 API is not available.");
	}

	Application::getInstance()->getInfoHardware();

	// Enable any OpenGL features we want to use
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	glViewport(0, 0, WIDHT, HEIGHT);

	// -------------------- Creamos la GUI -------------------- 
	Application::getInstance()->getGuiManager()->InitGUI(window);

	// -------------------- Registramos los callbacks -------------------- 
	setFunctionsCallbacks(window);

	// -------------------- Creamos los shaders -------------------- 
	ShaderManager::getInstance();

	// -------------------- Creamos la escena --------------------  
	std::cout << "Inicializando la escena..." << std::endl;
	Application::getInstance()->InitMainScene();

	std::cout << "Inicializando las texturas..." << std::endl;	
	//Bucle de carga
	bool isAllTextureLoaded = false;
	while (!glfwWindowShouldClose(window) && !isAllTextureLoaded) {
		float R = FloatAleatorio(0.1f, 1.0f);
		float G = FloatAleatorio(0.1f, 1.0f);
		float B = FloatAleatorio(0.1f, 1.0f);
		glClearColor(R, G, B, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		isAllTextureLoaded = Application::getInstance()->getTextureManager()->LoadTextures();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
	}

	std::cout << "Inicializando la GUI..." << std::endl;
	Application::getInstance()->InitGUI();

	Application::getInstance()->SetUniforms();

	// -------------------- RENDER LOOP --------------------  
	while (!glfwWindowShouldClose(window)) 
	{
		// Comprobamos si debemos bloquear o no el rat�n para el uso de la GUI.
		if (InputManager::getInstance()->getInputButtonDown(Key_F1)) {
			Application::getInstance()->wrappRaton = !Application::getInstance()->wrappRaton;
		}

		if (Application::getInstance()->getWrappRaton()) {
			//Hacemos que el cursor se quede bloqueado en medio y desaparezca.
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

 		Application::getInstance()->MainLoop(window);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Liberamos recursos
	Application::getInstance()->getGuiManager()->DestroyGUI();
	Application::getInstance()->DestroyInstance();
}

// --------------------------- FUNCIONES ----------------------------
float FloatAleatorio(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void setFunctionsCallbacks(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetKeyCallback(window, key_callback); --> Se hace en el render loop
	glfwSetCursorPosCallback(window, cursor_position_callback);

	//Hace que mientras se pulse una tecla, �sta devolver� GL_PRESS todo el rato.
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	Application::getInstance()->getMainScene()->framebuffer_size_callback(width, height);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (Application::getInstance()->getWrappRaton()) {
		Application::getInstance()->getMainScene()->cursor_position_callback(xpos, ypos);
	}
}