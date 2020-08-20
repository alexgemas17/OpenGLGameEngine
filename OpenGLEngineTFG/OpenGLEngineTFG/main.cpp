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

void showFPSCounter(int& nbFrames, double& lastTime);
void setFunctionsCallbacks(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
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
	/*glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, true);*/
	glfwSwapInterval(0); //VSync off
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//  -------------------- Iniciamos GLEW -------------------- 
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return -3;
	}

	// Especificamos la versión de GLEW para poder usar el Voxel Cone tracing.
	std::vector<std::pair<GLuint, std::string>> requiredGLEWExtensions = {
		{ GLEW_ARB_shader_image_load_store,		"ARB_shader_image_load_store"},
		{ GLEW_VERSION_4_5,						"GLEW_VERSION_4_5 (OpenGL 4.5)"},
		{ GL_ARB_multisample,					"GLFW MSAA" }
	};

	for (const auto& ext : requiredGLEWExtensions) {
		if (!ext.first) {
			std::cerr << "ERROR: " << ext.second << " not supported! Expect unexpected behaviour." << std::endl;
			std::cerr << "Press any key to continue ... " << std::endl;
			getchar();
		}
	}

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

	// -------------------- RENDER LOOP --------------------  
	while (!glfwWindowShouldClose(window)) {

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

void showFPSCounter(int &nbFrames, double &lastTime)
{
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
}

void setFunctionsCallbacks(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetKeyCallback(window, key_callback); --> Se hace en el render loop
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	//Hace que mientras se pulse una tecla, ésta devolverá GL_PRESS todo el rato.
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	Application::getInstance()->getMainScene()->framebuffer_size_callback(width, height);

	//window_refresh_callback(window);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	Application::getInstance()->getMainScene()->scroll_callback(xoffset, yoffset);
	//window_refresh_callback(window);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	//if (Application::getInstance()->getWrappRaton()) {
	//	//Hacemos que el cursor se quede bloqueado en medio y desaparezca.
	//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//	Application::getInstance()->getMainScene()->cursor_position_callback(xpos, ypos);

	//	window_refresh_callback(window);
	//}
	//else {
	//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//}
	//Hacemos que el cursor se quede bloqueado en medio y desaparezca.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Application::getInstance()->getMainScene()->cursor_position_callback(xpos, ypos);

	//window_refresh_callback(window);
}