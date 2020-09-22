#include "GuiManager.h"

#include <iostream>

#include "../Application.h"

GuiManager::GuiManager(): 
    show_demo_window(true), 
    show_another_window(true), 
    clear_color(ImVec4(0.45f, 0.55f, 0.60f, 1.00f)){}

void GuiManager::InitGUI(GLFWwindow* window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
}

void GuiManager::StartGUI()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GuiManager::DestroyGUI()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiManager::showGUI()
{
    //ImGui::ShowDemoWindow(&show_demo_window);

    static int numLights = Application::getInstance()->getMainScene()->NUM_LIGHTS;
    static int numObjTransp = Application::getInstance()->getMainScene()->NUM_TRANSP_OBJS;
    static float gamma = Application::getInstance()->getMainScene()->gamma;
    static float exposure = Application::getInstance()->getMainScene()->exposure;
    static int item_current = 0;
    static bool rotateObj = false;

    {
    	ImGui::Begin("Info data");

    	ImGui::Text("Numero de luces en la escena:");
        if (ImGui::SliderInt("Lighs", &numLights, 1, 2500)) 
        {
            Application::getInstance()->getMainScene()->AddNewNumLights(numLights);
        }
        ImGui::Spacing();

        if (ImGui::Combo("Type Render", &item_current, "Forward Rendering\0Deferred Rendering\0Forward Plus Rendering\0\0"))
        {
            Application::getInstance()->getMainScene()->setMode(item_current);
        }

        ImGui::Spacing(); 
        if (ImGui::SliderFloat("Gamma", &gamma, 0.1, 5.0f))
        {
            Application::getInstance()->getMainScene()->gamma = gamma;
        }
        ImGui::Spacing();
        if (ImGui::SliderFloat("Exposure", &exposure, 0.1f, 5.0f))
        {
            Application::getInstance()->getMainScene()->exposure = exposure;
        }

        ImGui::Spacing();
        ImGui::Text("Objetos transparentes:");
        if (ImGui::SliderInt("Objs", &numObjTransp, 0, 50))
        {
            Application::getInstance()->getMainScene()->NUM_TRANSP_OBJS = numObjTransp;
        }
        

        ImGui::Spacing();
    	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
    	ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
    	ImGui::End();
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiManager::addNodoSceneToGUI(NodoScene* nodo, std::string name)
{
    
}

// --------------------------- FUNCIONES PRIVADAS ----------------------------
void GuiManager::showMenu()
{
    
}

void GuiManager::recursiveAddNodoSceneToGUI(NodoScene* nodo)
{
    
}