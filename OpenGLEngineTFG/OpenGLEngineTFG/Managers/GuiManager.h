#pragma once

#include "../Render/SceneObj.h"
#include "../Render/NodoScene.h"

#include "../Gui/imgui.h"
#include "../Gui/imgui_impl_glfw.h"
#include "../Gui/imgui_impl_opengl3.h"

//SceneObj.h contiene, por heradar de Render, a glew.h
// Debe ponerse después de él.
#include <GLFW\glfw3.h>

struct ObjSceneInfo {
	SceneObj* obj;

	std::string NameObj;
	std::string NameTextureDiff;
	std::string NameTextureSpec;
	std::string NameTextureNormal;

	glm::vec3 Position;
	glm::vec3 RotationX;
	glm::vec3 RotationY;
	glm::vec3 RotationZ;
	glm::vec3 Scale;
};

class GuiManager
{
public:
	GuiManager();

	void InitGUI(GLFWwindow* window);
	void StartGUI();
	void showGUI();
	void DestroyGUI();

	void addNodoSceneToGUI(NodoScene* nodo, std::string name);

private:
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	std::vector<ObjSceneInfo> objsInfoGUI;

	void showMenu();

	void recursiveAddNodoSceneToGUI(NodoScene* nodo);
};