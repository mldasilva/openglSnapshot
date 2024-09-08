// #ifndef main_only
// #define main_only

#include "main.h"
#include "glmath.h"

#include "camera.h"
#include "controller.h"
#include "imGui_wrapper.h"
#include "model.h"
#include "gl_shaders.h"


// #endif 

int main(void)
{
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfw_hints();

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    float width = 1280;
    float height = 720;
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    /*m: init glew required to be after glfwMakeContextCurrent()
    glew site: First you need to create a valid OpenGL rendering context and call glewInit() to initialize the extension entry points*/ 
    if (GLEW_OK != glewInit()) return -1;

    init_glfw_debugger(window);

    // ===============================================================
    // main
    // ===============================================================
    imGui_wrapper _imguiWrapper(window);
    camera camera(width, height);



    controller controller(window, &camera);
    shader shader("../res/shaders/default.vs", "../res/shaders/default.fs");
    renderPool rp;

    model cube("../res/models/cone.gltf");
    model cone("../res/models/cube.gltf");
    
    
    rp.insert(cone.vertices, cone.indices, vec3(3,0,3));
    rp.insert(cube.vertices, cube.indices, vec3(1,0,-5));
    

    shader.create_ssbo(0, rp.matrices.size() * sizeof(mat4), rp.matrices.data());
    bufferObject bo(rp.vertices, 8, rp.indices, rp.commands);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        shader.draw(camera, bo);
        // camera.rotate(0.5f);
    
        _imguiWrapper.start_frame();
        _imguiWrapper.demo();
        _imguiWrapper.rendering();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }


    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}