#ifndef controller_h
#define controller_h

#include <GLFW/glfw3.h>
#include <iostream>

#include "camera.h"

class controller{
private:
    void static cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    void static mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void static key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void static framebuffer_size_callback(GLFWwindow* window, int width, int height);
public:
    controller(GLFWwindow *window, camera *camera);
    void mouse_controls(GLFWwindow *pWindow, float deltaTime, bool active);
};

#endif