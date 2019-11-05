#pragma once

#include <vector>
#include <glm.hpp>
#include <GLFW\glfw3.h>

enum KeyCode : int {
	Key_A = 65, 
	Key_B = 66,
	Key_C = 67,
	Key_D = 68,
	Key_E = 69,
	Key_F = 70,
	Key_G = 71,
	Key_H = 72,
	Key_I = 73,
	Key_J = 74,
	Key_K = 75,
	Key_L = 76,
	Key_M = 77,
	Key_N = 78,
	Key_O = 79,
	Key_P = 80,
	Key_Q = 81,
	Key_R = 82,
	Key_S = 83,
	Key_T = 84,
	Key_U = 85,
	Key_V = 86,
	Key_W = 87,
	Key_X = 88,
	Key_Y = 89,
	Key_Z = 90,
	Key_SPACE = 32,
	Key_LEFT_CONTROL = 341,
	Key_RIGHT_CONTROL = 345,
	Key_LEFT_SHIFT = 340,
	Key_RIGHT_SHIFT = 344,
	Key_LEFT_ALT = 342,
	Key_RIGHT_ALT = 346,
	Key_ENTER = 257,
	Key_TAB = 258,
	Key_ESC = 256
 };

class InputManager {
public:
	InputManager();
	~InputManager();

	/* Devuelve verdadero si la tecla que se ha pasado es la que se ha pulsado */
	bool getInputButtonDown(KeyCode key);

	/* Devuelve verdadero si la tecla que se ha pasado es la que se ha soltado */
	bool getInputButtonUp(KeyCode key);

	/* Devuelve verdadero si la tecla que se ha pasado sin importar si está presionada o no */
	bool getInputAnyButton(KeyCode key);

	/* Obtiene qué tecla se ha pulsado y almaneca los distintos estasdos de la tecla (pulsado, soltado, etc..) */
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static InputManager* getInstance();
private:
	// Instancia singleton.
	static InputManager* instance;

	bool lag;
	bool buttonUp;
	bool buttonDown;
	int keyCode;
};