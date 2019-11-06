#include <iostream>
#include <cstdlib>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm.hpp>

#include "PagShaderProgram.h"
#include "Render/Scene.h"
#include "Input/InputManager.h"

const float FOV = 45.0f;
const int WIDHT = 1024;
const int HEIGHT = 576;
const float ZNEAR = 0.1f;
const float ZFAR = 100.0f;

Scene* world; 
PagShaderProgram* basicShader;

void window_refresh_callback(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

int main() {

	//  -------------------- Iniciamos GLFW -------------------- 
	if (glfwInit() != GLFW_TRUE) {
		std::cout << "ERROR al inicializar GLFW" << std::endl;
		return -1;
	}

	//  -------------------- Configuramos GLFW -------------------- 
	glfwWindowHint(GLFW_SAMPLES, 4); // Activa antialiasing con 4 muestras.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	//  -------------------- Creamos la ventana de la app -------------------- 
	GLFWwindow* window = glfwCreateWindow(WIDHT, HEIGHT, "TFG UJA ENGINE", nullptr, nullptr);

	if (window == nullptr) {
		std::cout << "ERROR al abrir la ventada de GLFW" << std::endl;
		glfwTerminate();
		return -2;
	}
	glfwMakeContextCurrent(window);

	//  -------------------- Iniciamos GLEW -------------------- 
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return -3;
	}

	glViewport(0, 0, WIDHT, HEIGHT);

	//  -------------------- Info de la gráfica  -------------------- 
	std::cout << glGetString(GL_RENDERER) << std::endl;
	std::cout << glGetString(GL_VENDOR) << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// -------------------- Registramos los callbacks -------------------- 
	glfwSetWindowRefreshCallback(window, window_refresh_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	// -------------------- Creamos los shaders -------------------- 
	basicShader = new PagShaderProgram();
	//basicShader->createShaderProgram("Shaders/BasicShaderTexture");
	basicShader->createShaderProgram("Shaders/BasicShader");

	// -------------------- Creamos la escena --------------------  
	world = new Scene();
	world->InitCamara(FOV, WIDHT, HEIGHT, ZNEAR, ZFAR);
	world->InitObjs();

	// -------------------- RENDER LOOP --------------------  
	while (!glfwWindowShouldClose(window)) {
		//Inputs
		// Se obtienen los inputs desde el callback.

		//Update
		world->UpdateObjs();
		
		//Render
		world->DrawObjs(basicShader);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// -------------------- Liberamos recursos --------------------  
	delete world;
	delete basicShader;
}

// --------------------------- FUNCIONES ----------------------------
void window_refresh_callback(GLFWwindow* window) {
	//Llamamos a la función correspodiente a las acciones al refrescar la ventana.
	world->DrawObjs(basicShader);

	glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	world->framebuffer_size_callback(width, height);

	window_refresh_callback(window);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	//Actualizamos los estados del Input Manager
	InputManager::getInstance()->key_callback(window, key, scancode, action, mods);

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
	world->scroll_callback(xoffset, yoffset);
	window_refresh_callback(window);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	//Hacemos que el cursor se quede bloqueado en medio y desaparezca.
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//world->cursor_position_callback(xpos, ypos);

	window_refresh_callback(window);
}