#include <iostream>
#include <cstdlib>
// - IMPORTANTE: El include de Glew debe llamarse siempre ANTES de llamar al de GLFW.
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm.hpp>

void window_refresh_callback(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

int main() {

	// Iniciamos GLFW
	if (glfwInit() != GLFW_TRUE) {
		std::cout << "ERROR al inicializar GLFW" << std::endl;
		return -1;
	}

	// Configuramos GLFW
	glfwWindowHint(GLFW_SAMPLES, 4); // Activa antialiasing con 4 muestras.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// Creamos la ventana de la app.
	GLFWwindow* window = glfwCreateWindow(1024, 576, "TFG UJA ENGIE", nullptr, nullptr);

	if (window == nullptr) {
		std::cout << "ERROR al abrir la ventada de GLFW" << std::endl;
		glfwTerminate();
		return -2;
	}

	// Hace que la ventana que se pasa como argumento use el contexto de GLFW cuyas características se han definido previamente.
	glfwMakeContextCurrent(window);

	// Iniciamos GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return -3;
	}

	// Vemos las propiedades de la máquina
	std::cout << glGetString(GL_RENDERER) << std::endl;
	std::cout << glGetString(GL_VENDOR) << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;


	// Registramos los callbacks
	glfwSetWindowRefreshCallback(window, window_refresh_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	// Iniciamos la escena
	// Scene->Init();

	// ---- RENDER LOOP ----
	while (!glfwWindowShouldClose(window)) {
		//Inputs

		//Render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwPollEvents();
	}

	// Liberamos recursos.
}



// - Esta función callback será llamada cada vez que el área de dibujo OpenGL deba ser redibujada.
void window_refresh_callback(GLFWwindow* window) {
	//Llamamos a la función correspodiente a las acciones al refrescar la ventana.
	//PagRenderer::getInstance()->refreshCallback();

	// - GLFW usa un doble buffer para que no haya parpadeo. Esta orden
	// intercambia el buffer back (que se ha estado dibujando) por el
	// que se mostraba hasta ahora front.
	glfwSwapBuffers(window);
}
// - Esta función callback será llamada cada vez que se cambie el tamaño del área de dibujo OpenGL.
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	//PagRenderer::getInstance()->size_callback(width, height);

	window_refresh_callback(window);
}
// - Esta función callback será llamada cada vez que se pulse una tecla dirigida al área de dibujo OpenGL.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	window_refresh_callback(window);
}

// - Esta función callback será llamada cada vez que se pulse algún botón del ratón sobre el área de dibujo OpenGL.
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		//PagRenderer::getInstance()->ratonPress(button);
	}
	else if (action == GLFW_RELEASE) {
		//PagRenderer::getInstance()->ratonRelease(button);
	}
}

// - Esta función callback será llamada cada vez que se mueva la rueda del ratón sobre el área de dibujo OpenGL.
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	//PagRenderer::getInstance()->scroll_callback(xoffset, yoffset);
	window_refresh_callback(window);
}

// - Esta función callback será llamada cada vez que se mueva el ratón sobre el área de dibujo OpenGL.
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//PagRenderer::getInstance()->cursor_position_callback(xpos, ypos);
	window_refresh_callback(window);
}