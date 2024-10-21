#ifndef controller_h
#define controller_h

#include <glfw3.h>
#include "camera.h"
#include "controller_interface.h"

class Controller
{
private:
    // Camera *_camera;
    void static cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    void static mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void static key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void static framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void static window_position_callback(GLFWwindow* window, int xpos, int ypos);

    controllerI* data; // controller writes to this pointer that points to interface

    int windowWidth, windowHeight;
    double mouseX, mouseY;


public:
    Controller(GLFWwindow *window, Camera *camera);
    ~Controller();
    
    controllerI interface; // access to interface from other classes

    void mouse_controls(GLFWwindow *pWindow, float deltaTime, bool active);
    // vec3 mouseScreenToWorld(vec2 inMouse);
};

#endif