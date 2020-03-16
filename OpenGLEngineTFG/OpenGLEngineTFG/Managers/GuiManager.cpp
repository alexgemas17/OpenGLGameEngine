#include "GuiManager.h"

#include <iostream>

GuiManager::GuiManager(): 
    show_demo_window(true), 
    show_another_window(true), 
    clear_color(ImVec4(0.45f, 0.55f, 0.60f, 1.00f)) {}

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
    ImGui_ImplOpenGL3_Init("#version 400");

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
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
	//showMenu();

    //ImGui::ShowDemoWindow(&show_demo_window);

    //TO-DO: HACER FUNCIONES DE LA UI
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
    	static float f = 0.0f;
    	static int counter = 0;

    	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    	ImGui::Checkbox("Another Window", &show_another_window);

        static float translation[] = { 0.0, 0.0 };
        static float translation2[] = { 0.0, 0.0 };
        static float translation3[] = { 0.0, 0.0 };
        

        if (ImGui::CollapsingHeader("Objetos??"))
        {
            ImGui::Text("Objetos del mundo:");
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Nodo 1"))
            {
                if (ImGui::CollapsingHeader("Hijo"))
                {
                    ImGui::Separator();
                    if (ImGui::SliderFloat2("position", translation, -1.0, 1.0)) {
                        std::cout << "Modificado 1" << std::endl;
                    }
                }
            }

            if (ImGui::CollapsingHeader("Nodo 2"))
            {
                if (ImGui::CollapsingHeader("Hijo 1"))
                {
                    ImGui::Separator();
                    if (ImGui::SliderFloat2("position", translation2, -1.0, 1.0)) {
                        std::cout << "Modificado 2" << std::endl;
                    }
                }
                if (ImGui::CollapsingHeader("Hijo 2"))
                {
                    ImGui::Separator();
                    if (ImGui::SliderFloat2("position", translation3, -1.0, 1.0)) {
                        std::cout << "Modificado 3" << std::endl;
                    }
                }

            }
        }

    	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    		counter++;
    	ImGui::SameLine();
    	ImGui::Text("counter = %d", counter);

    	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    	ImGui::End();
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiManager::showMenu()
{
    //IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!"); // Exceptionally add an extra assert here for people confused with initial dear imgui setup

    //// Main body of the Demo window starts here.
    //if (!ImGui::Begin("Dear ImGui Demo"))
    //{
    //    // Early out if the window is collapsed, as an optimization.
    //    ImGui::End();
    //    return;
    //}

    //// Menu Bar
    //if (ImGui::BeginMenuBar())
    //{
    //    if (ImGui::BeginMenu("Menu"))
    //    {
    //        //ShowExampleMenuFile();
    //        ImGui::EndMenu();
    //    }
    //    if (ImGui::BeginMenu("Examples"))
    //    {
    //        /*ImGui::MenuItem("Main menu bar", NULL, &show_app_main_menu_bar);
    //        ImGui::MenuItem("Console", NULL, &show_app_console);
    //        ImGui::MenuItem("Log", NULL, &show_app_log);
    //        ImGui::MenuItem("Simple layout", NULL, &show_app_layout);
    //        ImGui::MenuItem("Property editor", NULL, &show_app_property_editor);
    //        ImGui::MenuItem("Long text display", NULL, &show_app_long_text);
    //        ImGui::MenuItem("Auto-resizing window", NULL, &show_app_auto_resize);
    //        ImGui::MenuItem("Constrained-resizing window", NULL, &show_app_constrained_resize);
    //        ImGui::MenuItem("Simple overlay", NULL, &show_app_simple_overlay);
    //        ImGui::MenuItem("Manipulating window titles", NULL, &show_app_window_titles);
    //        ImGui::MenuItem("Custom rendering", NULL, &show_app_custom_rendering);
    //        ImGui::MenuItem("Documents", NULL, &show_app_documents);*/
    //        ImGui::EndMenu();
    //    }
    //    if (ImGui::BeginMenu("Tools"))
    //    {
    //        /*ImGui::MenuItem("Metrics", NULL, &show_app_metrics);
    //        ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
    //        ImGui::MenuItem("About Dear ImGui", NULL, &show_app_about);*/
    //        ImGui::EndMenu();
    //    }
    //    ImGui::EndMenuBar();
    //}

    //ImGui::Text("dear imgui says hello. (%s)", IMGUI_VERSION);
    //ImGui::Spacing();
}