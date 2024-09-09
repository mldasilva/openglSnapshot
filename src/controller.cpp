#include "controller.h"

void controller::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
    {
        
        // Retrieve the application instance from the window's user pointer
        // camera* ptr = static_cast<camera*>(glfwGetWindowUserPointer(window));
        // if (ptr) {
        //     // Call the appropriate member function
        //     cout << ptr->position << endl;
        //     ptr->move(vec3(0.1f,0,0.1f));
        // }
        
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

void controller::window_position_callback(GLFWwindow* window, int xpos, int ypos) {
    std::cout << "Window moved to position: (" << xpos << ", " << ypos << ")" << std::endl;
}

void controller::mouse_controls(GLFWwindow *window, float deltaTime, bool active)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && active) 
    {
        // get mouse world ray (origin and direction)
        double mouseX, mouseY;
        int width, height;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        glfwGetWindowSize(window, &width, &height);

        // Convert mouse position to NDC
        float ndcX = (2.0f * mouseX) / width - 1.0f;
        float ndcY = 1.0f - (2.0f * mouseY) / height; // Invert Y-axis

        // Create NDC positions for near and far points (Z = -1 for near, 1 for far)
        glm::vec4 rayClipNear(ndcX, ndcY, -1.0f, 1.0f);
        glm::vec4 rayClipFar(ndcX, ndcY, 1.0f, 1.0f);

        // Convert from clip space to view space
        glm::mat4 inverseProjection = glm::inverse(_camera->projection);
        glm::vec4 rayViewNear = inverseProjection * rayClipNear;
        glm::vec4 rayViewFar = inverseProjection * rayClipFar;
        rayViewNear /= rayViewNear.w; // Perspective divide
        rayViewFar /= rayViewFar.w;

        // Convert from view space to world space
        glm::mat4 inverseView = glm::inverse(_camera->view);
        glm::vec4 rayWorldNear = inverseView * rayViewNear;
        glm::vec4 rayWorldFar = inverseView * rayViewFar;

        // Ray origin is the near point, direction is the vector from near to far
        vec3 rayOrigin = glm::vec3(rayWorldNear);
        vec3 rayDirection = glm::normalize(glm::vec3(rayWorldFar - rayWorldNear));

        /* plane intersection */

        // Plane equation: y = 0
        float t = -rayOrigin.y / rayDirection.y;

        // If t < 0, the intersection is behind the ray origin, so ignore it
        if (t < 0.0f) {
            // cout << "false" << endl;
        }

        // Calculate the intersection point
        vec3 intersectionPoint = rayOrigin + t * rayDirection;

        vec3 d = normalize(intersectionPoint - _camera->target);
        
        float speed = 6.0f;
        _camera->move(vec3(d.x * deltaTime * speed, 0 , d.z * deltaTime * speed));
        
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && active) 
    {
        _camera->rotate(2.0f);
    }
}

controller::controller(GLFWwindow *window, camera *camera)
{
    cout << "creating controller object..." << endl;
    // glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    _camera = camera;

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

    // Set the window position callback
    glfwSetWindowPosCallback(window, window_position_callback);
}

controller::~controller()
{
    cout << "deleting controller object..." << endl;
}
