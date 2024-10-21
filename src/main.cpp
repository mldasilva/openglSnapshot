// #ifndef main_only
// #define main_only
#include <iostream>
// #include "main.h"
// #include "glmath.h"

// #include "camera.h"
// #include "controller.h"
// #include "imGui_wrapper.h"
// #include "model.h"
// #include "gl_shaders.h"

// #include <PxPhysicsAPI.h>

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
    std::cout << " hello world"<< std::endl;
    // GLFWwindow* window;
    // /* Initialize the library */
    // if (!glfwInit())
    //     return -1;

    // glfw_hints();

    // /* Create a windowed mode window and its OpenGL context */
    // window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
    // if (!window)
    // {
    //     glfwTerminate();
    //     return -1;
    // }

    // float width = 1280;
    // float height = 720;
    
    // float deltaTime = 0.0f;
    // double currentTime, lastTick;

    // bool imGuiHovered = false;

    // /* Make the window's context current */
    // glfwMakeContextCurrent(window);
    // glfwSwapInterval(1); // Enable vsync

    // /*m: init glew required to be after glfwMakeContextCurrent()
    // glew site: First you need to create a valid OpenGL rendering context and call glewInit() to initialize the extension entry points*/ 
    // if (GLEW_OK != glewInit()) return -1;

    // init_glfw_debugger(window);

    // // ===============================================================
    // // main
    // // ===============================================================
    // camera          camera(width, height);
    // controller      controller(window, &camera);    // after camera
    // imGui_wrapper   _imguiWrapper(window);          // after controller
    // shader          shader("default.vs", "default.fs");
    // renderPool rp;

    // model cube("../res/models/cube.gltf");
    // model cone("../res/models/cube.gltf");

    // rp.insert(cone.vertices, cone.indices, vec3(0, 0, 0));
    // rp.insert(cube.vertices, cube.indices, vec3(1, 0, -12));
    // rp.insert(cube.vertices, cube.indices, vec3(8, 0, 4));

    // shader.create_ssbo(0, rp.matrices.size() * sizeof(mat4), rp.matrices.data());
    // bufferObject bo(rp.vertices, 8, rp.indices, rp.commands);

    // // ===============================================================
    // // textures
    // // ===============================================================
    
    // texture textures;
    // textures.loadTexture("../res/textures/dice.png", 1);
    // textures.loadTexture("../res/textures/greentop.png", 2);
    // textures.loadTexture("../res/textures/greentop.png");
    // textures.loadTexture("../res/textures/dice.png");

    // shader.create_ssbo(2, textures.handleByteSize(), textures.handles());

    // // ===============================================================
    // // blending settings
    // // ===============================================================
    
    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // // physics
    // // Initialize PhysX foundation and physics SDK
    // // static PxDefaultAllocator gAllocator;
    // // static PxDefaultErrorCallback gErrorCallback;
    // // PxFoundation* foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    // // PxPhysics* physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());

    // // // Create a default dispatcher, required for simulation
    // // PxDefaultCpuDispatcher* dispatcher = PxDefaultCpuDispatcherCreate(2);

    // // // Create the scene description
    // // PxSceneDesc sceneDesc(physics->getTolerancesScale());
    // // sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);  // Set gravity
    // // sceneDesc.cpuDispatcher = dispatcher;
    // // sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    // // // Create the scene
    // // PxScene* scene = physics->createScene(sceneDesc);

    // // // Create a default material (friction, restitution, etc.)
    // // PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.6f);

    // // // Create a static ground plane
    // // PxRigidStatic* groundPlane = PxCreatePlane(*physics, PxPlane(0, 1, 0, 0), *material);
    // // scene->addActor(*groundPlane);

    // // // Create a dynamic box (cube)
    // // PxReal boxSize = 1.0f;
    // // PxBoxGeometry geometry(boxSize, boxSize, boxSize);
    // // PxTransform transform(PxVec3(0.0f, 10.0f, 0.0f));  // Place the cube 10 units above the ground
    // // PxRigidDynamic* box = PxCreateDynamic(*physics, transform, geometry, *material, 10.0f);
    // // box->setAngularDamping(0.5f);  // Damping to prevent excessive rotation
    // // scene->addActor(*box);

    // // // Simulate the scene
    // // const PxReal timeStep = 1.0f / 60.0f;  // Simulate at 60 FPS
    // // for (int i = 0; i < 300; ++i) {
    // //     scene->simulate(timeStep);  // Step the simulation
    // //     scene->fetchResults(true);  // Wait for the simulation results

    // //     // Get the current position of the box
    // //     PxTransform boxTransform = box->getGlobalPose();
    // //     PxVec3 pos = boxTransform.p;
    // //     printf("Box position: x = %f, y = %f, z = %f\n", pos.x, pos.y, pos.z);
    // // }


    // /* Loop until the user closes the window */
    // while (!glfwWindowShouldClose(window))
    // {
    //     lastTick = currentTime;
    //     currentTime = glfwGetTime();
    //     deltaTime = currentTime - lastTick;

    //     /* controls here */ 
    //     controller.mouse_controls(window, deltaTime, !imGuiHovered);

    //     /* Render here */
    //     glClearColor(0.0f, 0.2f, 0.3f, 0.1f);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    //     shader.draw(camera, bo);
    
    //     // _imguiWrapper.start_frame();
    //     // _imguiWrapper.demo(&imGuiHovered);
    //     // _imguiWrapper.rendering();

    //     /* Swap front and back buffers */
    //     glfwSwapBuffers(window);

    //     /* Poll for and process events */
    //     glfwPollEvents();
    // }
    // cout << endl;
    // GLenum error = glGetError();
    // if (error != GL_NO_ERROR) {
    //     std::cerr << "OpenGL Error: " << error << std::endl;
    // }

    // // Clean up
    // // box->release();
    // // groundPlane->release();
    // // scene->release();
    // // dispatcher->release();
    // // physics->release();
    // // foundation->release();

    // glfwDestroyWindow(window);
    // glfwTerminate();



    return 0;
}