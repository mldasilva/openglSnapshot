#ifndef controller_h
#define controller_h

#include <glfw3.h>
#include "camera.h"

struct controllerState{
    bool isJumping;
};

class controller
{
private:
    camera *_camera;
    void static cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    void static mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void static key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void static framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void static window_position_callback(GLFWwindow* window, int xpos, int ypos);

public:
    controller(GLFWwindow *window, camera *camera);
    ~controller();
    
    controllerState cs;

    void mouse_controls(GLFWwindow *pWindow, float deltaTime, bool active, vec3 *outMouseDirection);
};

#endif