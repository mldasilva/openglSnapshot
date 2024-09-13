// #ifndef main_only
// #define main_only

#include "main.h"
#include "glmath.h"

#include "camera.h"
#include "controller.h"
#include "imGui_wrapper.h"
#include "model.h"
#include "gl_shaders.h"

#include <Jolt.h>

// Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

// using namespace physx;
// #endif 

//big things:
// ---------------
// lights
// shadows
// wfc - wave function collapse
// particle engine
// physics engine
// sprite / billboard
// animation

//smaller things:
//------------------
// skills, abilities
// quest
// ai
// combat

// huge things:
//------------------
// networking

// know issue:
// dragging window freezes game and messes up deltatime

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
    
    float deltaTime = 0.0f;
    double currentTime = 0;
    double lastTick = 0;

    bool imGuiHovered = false;

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
    camera          camera(width, height);
    controller      controller(window, &camera);    // after camera
    imGui_wrapper   _imguiWrapper(window);          // after controller
    shader          shader("res/shaders/default.vs", "res/shaders/default.fs");
    renderPool rp;

    model cube("res/models/cube.gltf");
    model cone("res/models/cube.gltf");

    rp.insert(cone.vertices, cone.indices, vec3(0, 0, 0));
    rp.insert(cube.vertices, cube.indices, vec3(1, 0, -12));
    rp.insert(cube.vertices, cube.indices, vec3(8, 0, 4));

    shader.create_ssbo(0, rp.matrices.size() * sizeof(mat4), rp.matrices.data());
    bufferObject bo(rp.vertices, 8, rp.indices, rp.commands);

    // ===============================================================
    // textures
    // ===============================================================
    
    texture textures;
    textures.loadTexture("res/textures/dice.png", 1);
    textures.loadTexture("res/textures/greentop.png", 2);
    textures.loadTexture("res/textures/greentop.png");
    textures.loadTexture("res/textures/dice.png");

    shader.create_ssbo(2, textures.handleByteSize(), textures.handles());

    // ===============================================================
    // blending settings
    // ===============================================================
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        lastTick = currentTime;
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTick;

        /* controls here */ 
        controller.mouse_controls(window, deltaTime, !imGuiHovered);

        /* Render here */
        glClearColor(0.0f, 0.2f, 0.3f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.draw(camera, bo);
    
        _imguiWrapper.start_frame();
        _imguiWrapper.demo(&imGuiHovered);
        _imguiWrapper.rendering();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // cout << endl;
    // GLenum error = glGetError();
    // if (error != GL_NO_ERROR) {
    //     std::cerr << "OpenGL Error: " << error << std::endl;
    // }

    glfwDestroyWindow(window);
    glfwTerminate();



    return 0;
}