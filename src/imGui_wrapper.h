#ifndef imGui_wrapper_h
#define imGui_wrapper_h

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include <iostream>

class imGui_wrapper {
private:
    // Our state
    bool show_demo_window = true;
    // bool show_another_window = false;
    // ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    GLFWwindow *windowRef;

public:
    // Constructor
    imGui_wrapper(GLFWwindow* window);

    // Destructor
    ~imGui_wrapper();

    // Member function prototypes
    void start_frame();
    void demo();
    void rendering();
};


#endif