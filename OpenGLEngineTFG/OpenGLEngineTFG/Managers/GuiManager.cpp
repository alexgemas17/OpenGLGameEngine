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
    static float gamma = Application::getInstance()->getMainScene()->gamma;
    static float exposure = Application::getInstance()->getMainScene()->exposure;
    static int item_current = 0;

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
    recursiveAddNodoSceneToGUI(nodo);
}

// --------------------------- FUNCIONES PRIVADAS ----------------------------
void GuiManager::showMenu()
{
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!"); // Exceptionally add an extra assert here for people confused with initial dear imgui setup

    // Main body of the Demo window starts here.
    if (!ImGui::Begin("Dear ImGui Demo"))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    // Menu Bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            //ShowExampleMenuFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Examples"))
        {
            /*ImGui::MenuItem("Main menu bar", NULL, &show_app_main_menu_bar);
            ImGui::MenuItem("Console", NULL, &show_app_console);
            ImGui::MenuItem("Log", NULL, &show_app_log);
            ImGui::MenuItem("Simple layout", NULL, &show_app_layout);
            ImGui::MenuItem("Property editor", NULL, &show_app_property_editor);
            ImGui::MenuItem("Long text display", NULL, &show_app_long_text);
            ImGui::MenuItem("Auto-resizing window", NULL, &show_app_auto_resize);
            ImGui::MenuItem("Constrained-resizing window", NULL, &show_app_constrained_resize);
            ImGui::MenuItem("Simple overlay", NULL, &show_app_simple_overlay);
            ImGui::MenuItem("Manipulating window titles", NULL, &show_app_window_titles);
            ImGui::MenuItem("Custom rendering", NULL, &show_app_custom_rendering);
            ImGui::MenuItem("Documents", NULL, &show_app_documents);*/
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {
            /*ImGui::MenuItem("Metrics", NULL, &show_app_metrics);
            ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
            ImGui::MenuItem("About Dear ImGui", NULL, &show_app_about);*/
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("dear imgui says hello. (%s)", IMGUI_VERSION);
    ImGui::Spacing();

    /*
    
        if (ImGui::CollapsingHeader("Objetos??"))
        {
            ImGui::Text("Objetos del mundo:");
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Nodo 1"))
            {
                if (ImGui::CollapsingHeader("Hijo"))
                {
                    //ImGui::Image();
                }
            }

            if (ImGui::CollapsingHeader("Nodo 2"))
            {
                if (ImGui::CollapsingHeader("Hijo 1"))
                {
                    ImGui::Separator();
                    if (ImGui::SliderFloat2("position 2", translation2, -1.0, 1.0)) {
                        std::cout << "Modificado 2" << std::endl;
                    }
                }
                if (ImGui::CollapsingHeader("Hijo 2"))
                {
                    ImGui::Separator();
                    if (ImGui::SliderFloat2("position 3", translation3, -1.0, 1.0)) {
                        std::cout << "Modificado 3" << std::endl;
                    }
                }
            }
        }
    */
}

void GuiManager::recursiveAddNodoSceneToGUI(NodoScene* nodo)
{
    
}