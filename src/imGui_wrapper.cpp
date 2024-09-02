#include "imGui_wrapper.h"

//==================================================================
// imgui helper functions
//==================================================================


imGui_wrapper::imGui_wrapper(GLFWwindow* window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    // std::cout << "constructor" << std::endl;

    windowRef = window;
}

imGui_wrapper::~imGui_wrapper()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // std::cout << "deconstructor" << std::endl;
}

void imGui_wrapper::start_frame()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void imGui_wrapper::demo()
{
    if (show_demo_window)
    {
        ImGui::ShowDemoWindow(&show_demo_window);
    }  
}

void imGui_wrapper::rendering()
{
    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(windowRef, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    // glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    // glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

