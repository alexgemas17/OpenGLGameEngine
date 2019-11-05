#include "InputManager.h"

InputManager* InputManager::instance = nullptr;

InputManager::InputManager(): buttonUp(false), buttonDown(false), keyCode(-1), lag(false) {}

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
	if (key == this->keyCode && this->buttonDown && !lag) {
		lag = true;
		return true;
	}
	else {
		return false;
	}
}

bool InputManager::getInputButtonUp(KeyCode key)
{
	if (key == this->keyCode && this->buttonUp) return true;
	return false;
}

bool InputManager::getInputAnyButton(KeyCode key)
{
	if (key == this->keyCode && (this->buttonUp || this->buttonDown)) return true;
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