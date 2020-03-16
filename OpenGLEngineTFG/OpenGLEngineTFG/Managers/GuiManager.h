#pragma once

#include <GLFW\glfw3.h>
#include <string>

#include "../Gui/imgui.h"
#include "../Gui/imgui_impl_glfw.h"
#include "../Gui/imgui_impl_opengl3.h"

struct ObjSceneInfo {
	std::string nameObj;

};

class GuiManager
{
public:
	GuiManager();

	void InitGUI(GLFWwindow* window);
	void StartGUI();
	void showGUI();
	void DestroyGUI();


private:
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	void showMenu();
};