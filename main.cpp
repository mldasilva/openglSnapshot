// #ifndef main_only
// #define main_only

#include "main.h"
// #include "glmath.h"

#include "controller.h"
#include "gl_shaders.h"
#include "jolt_wrapper.h"
#include "imGui_wrapper.h"

//big things:
// ---------------
// lights
// shadows
// wfc - wave function collapse
// sprite / billboard

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
// multiple different functions to open files to strings

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

    string vertexPath   = string(SHADER_DIR) + "/default.vs";
    string fragmentPath = string(SHADER_DIR) + "/default.fs";

    string vertexPathJolt   = string(SHADER_DIR) + "/default_jolt.vs";
    string fragmentPathJolt = string(SHADER_DIR) + "/default_jolt.fs";

    string vertexPathbb   = string(SHADER_DIR) + "/default_bb.vs"; // billboard
    string fragmentPathbb = string(SHADER_DIR) + "/default_bb.fs";

    string cubePath     = string(MODELS_DIR) + "/cube.gltf";
    string conePath     = string(MODELS_DIR) + "/cube.gltf";

    string texturesPath = string(TEXTURES_DIR);

    // ===============================================================
    // main
    // ===============================================================

    camera          camera(width, height);
    controller      controller(window, &camera);    // after camera
    imGui_wrapper   imgui(window);                  // after controller

    shader          shader_main(vertexPath.c_str(), fragmentPath.c_str());
    renderPool      render_main;

    shader          shader_jolt(vertexPathJolt.c_str(), fragmentPathJolt.c_str());
    renderPool      render_jolt;

    shader          shader_bb(vertexPathbb.c_str(), fragmentPathbb.c_str());
    renderPool      render_bb;

    joltWrapper     jolt; // Now we can create the actual physics system.

    model cube(cubePath.c_str());
    model cone(conePath.c_str());

    render_main.insert(cone.vertices, cone.indices, vec3(0, -1, 0));

    // for (size_t i = 0; i < 10; i++)
    // {
    //     /* code */
    //     jolt.create_object(render_jolt, enviroment_dynamic, cube, Vec3((i*2), 5, 0), Quat::sIdentity());
    // }
      
    // floor
    BodyID floor = jolt.create_shape(new BoxShape(Vec3(100.0f, 1.0f, 100.0f)), false, Vec3(0.0, -1.0, 0.0));

    jolt.create_object(render_jolt, enviroment_static, cube, Vec3(3, 1, 3), Quat::sIdentity());
    jolt.create_object(render_jolt, enviroment_static, cube, Vec3(0.5f, 1, 3), Quat::sIdentity());
    // create player
    playerController pc(render_bb, &jolt, cube, Vec3(-3.0f,10.0f,0.0f));
    
    MyBodyFilter floorFilter(floor);
    jolt.pBodyFilter = &floorFilter;
    // ===============================================================
    // textures
    // ===============================================================
    
    texture textures;
    textures.loadTexture((texturesPath + "/dice.png").c_str(), 1);
    textures.loadTexture((texturesPath + "/greentop.png").c_str(), 2);
    textures.loadTexture((texturesPath + "/greentop.png").c_str());
    textures.loadTexture((texturesPath + "/dice.png").c_str());

    // ===============================================================
    // blending settings
    // ===============================================================
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // ===============================================================
    // finalizing before scene
    // ===============================================================
    // jolt optimize shader ssbo creation and bufferObject must be after object creation,

	jolt.optimize();

    shader_main.create_ssbo(0, render_main.matrices.size() * sizeof(mat4), render_main.matrices.data());
    shader_main.create_ssbo(2, textures.handleByteSize(), textures.handles());
    bufferObject bo(render_main.vertices, 8, render_main.indices, render_main.commands);

    shader_jolt.create_ssbo(1, jolt.matrices.size() * sizeof(mat4), jolt.matrices.data());
    shader_jolt.create_ssbo(3, textures.handleByteSize(), textures.handles());
    bufferObject bo_jolt(render_jolt.vertices, 8, render_jolt.indices, render_jolt.commands);

    shader_bb.create_ssbo(4, pc.matrices.size() * sizeof(mat4), pc.matrices.data());
    shader_bb.create_ssbo(5, textures.handleByteSize(), textures.handles());
    bufferObject bo_player(render_bb.vertices, 8, render_bb.indices, render_bb.commands);


    vec3 inputDirection = vec3();
    std::cout << "hello world.." << std::endl;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        lastTick = currentTime;
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTick;

        /* controls here */     
        controller.mouse_controls(window, deltaTime, !imGuiHovered, &inputDirection);       
        pc.update(v(inputDirection), controller.cs.isJumping, deltaTime); // player controller
        camera.moveTo(v(pc.position));

        /* physics */
        jolt.update(); // physics tick

        shader_jolt.update_ssbo(0, jolt.matrices.size() * sizeof(mat4), jolt.matrices.data());
        shader_bb.update_ssbo(0, pc.matrices.size() * sizeof(mat4), pc.matrices.data());
        
        /* Render here */
        glClearColor(0.0f, 0.2f, 0.3f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader_main.draw(camera, bo);
        shader_jolt.draw(camera, bo_jolt);
        shader_bb.draw(camera, bo_player);

        imgui.start_frame();
        imgui.demo(&imGuiHovered);
        imgui.rendering();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    cout << endl;
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }
    glfwDestroyWindow(window);
    glfwTerminate();



    return 0;
}