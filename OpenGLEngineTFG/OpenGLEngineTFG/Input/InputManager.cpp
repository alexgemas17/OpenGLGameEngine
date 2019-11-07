#include "InputManager.h"

InputManager* InputManager::instance = nullptr;

InputManager::InputManager(): buttonUp(false), buttonDown(false), keyCode(-1), lag(false), window(nullptr) {}

InputManager::~InputManager() {}

// Acceder al singleton.
InputManager* InputManager::getInstance()
{
	if (!instance) {
		instance = new InputManager();
	}
	return instance;
}

bool InputManager::getInputButtonDown(KeyCode key)
{
	if (window) {
		if (glfwGetKey(window, key) == GLFW_PRESS) {
			this->buttonDown = true;
			this->buttonUp = false;
			return true;
		}
		else {
			return false;
		}
	}
}

bool InputManager::getInputButtonUp(KeyCode key)
{
	if (window) {
		if (glfwGetKey(window, key) == GLFW_RELEASE) {
			this->buttonDown = false;
			this->buttonUp = true;
			return true;
		}
		else {
			return false;
		}
	}
}

bool InputManager::getInputAnyStateButton(KeyCode key)
{
	if (window) {
		if (glfwGetKey(window, key) == GLFW_PRESS || glfwGetKey(window, key) == GLFW_RELEASE) {
			return true;
		}
		else {
			return false;
		}
	}
}

bool InputManager::isAnyButtonPressed()
{
	if (this->buttonDown || this->buttonUp) return true;
	return false;
}

void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		lag = false;
		this->keyCode = key;
		this->buttonDown = true;

	}else if (action == GLFW_RELEASE) {
		this->buttonDown = false;
		this->keyCode = -1;
	}
}

void InputManager::key_callback(GLFWwindow* window)
{
	this->window = window;
}