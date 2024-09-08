#include "controller.h"

void controller::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
    {

    }    
}

void controller::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // printf("mouse click \n");
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        // Retrieve the application instance from the window's user pointer
        camera* app = static_cast<camera*>(glfwGetWindowUserPointer(window));
        if (app) {
            // Call the appropriate member function
            cout << app->position << endl;
        }
    }
}

void controller::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // action == GLFW_PRESS makes it only fire once on down stroke
    if(key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, 1);
    }
    if(key == GLFW_KEY_R && action == GLFW_PRESS)
    {

    }
}

void controller::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void controller::mouse_controls(GLFWwindow *pWindow, float deltaTime, bool active)
{
    if (glfwGetMouseButton(pWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && active) 
    {
        double mouseX, mouseY;
        glfwGetCursorPos(pWindow, &mouseX, &mouseY);
    }
}

controller::controller(GLFWwindow *window, camera *camera)
{
    // glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    // Set the user pointer to the camera instance
    glfwSetWindowUserPointer(window, camera);

    // mouse position
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // mouse buttons
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // keyboard
    glfwSetKeyCallback(window, key_callback);

    //viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

