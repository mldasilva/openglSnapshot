#include "main.h"
// #include "glmath.h"

#include "controller.h"
#include "animator.h"
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

    // ===============================================================
    // main
    // ===============================================================

    Camera          camera(width, height);
    Controller      controller(window, &camera);    // after camera
    imGui_wrapper   imgui(window);                  // after controller

    shader          shader_main(shader_default_vs, shader_default_fs);
    RenderPool      render_main;

    shader          shader_jolt(shader_jolt_vs, shader_jolt_fs);
    RenderPool      render_jolt;

    shader          shader_bb(shader_bb_vs, shader_bb_fs);
    RenderPool      render_bb;

    JoltWrapper     jolt; // Now we can create the actual physics system.

    // ===============================================================
    // model loading
    // ===============================================================

    Model cube(model_cube);
    Model cone(model_cone);
    
    render_main.insert(cone.vertices, cone.indices, vec3(0, -1, 0));
    
    for (size_t i = 0; i < 10; i++)
    {
        /* code */
        jolt.create_object(render_jolt, enviroment_dynamic, cube, Vec3((i*2), 5, 0), Quat::sIdentity());
    }
      
    // floor
    BodyID floor = jolt.create_shape(new BoxShape(Vec3(100.0f, 1.0f, 100.0f)), false, Vec3(0.0, -1.0, 0.0));

    jolt.create_object(render_jolt, enviroment_static, cube, Vec3(3, 1, 3), Quat::sIdentity());
    jolt.create_object(render_jolt, enviroment_static, cube, Vec3(0.5f, 1, 3), Quat::sIdentity());
    
    // create player
    PlayerController playerController(render_bb, &jolt, Vec3(-3.0f,10.0f,0.0f));
    
    // add floor to joly body filter
    MyBodyFilter floorFilter(floor);
    jolt.pBodyFilter = &floorFilter;
    
    // ===============================================================
    // textures
    // ===============================================================
    
    texture textures;
    textures.loadTexture(texture_anim_00);
    textures.loadTexture(texture_dice);

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
    bufferObject bo_jolt(render_jolt.vertices, 8, render_jolt.indices, render_jolt.commands);

    shader_bb.create_ssbo(4, sizeof(Vec4), &playerController.position);
    shader_bb.create_ssbo(6, playerController.textureIndices.size() * sizeof(uint), playerController.textureIndices.data());
    bufferObject bo_player(render_bb.vertices, 8, render_bb.indices, render_bb.commands);

    std::cout << "hello world.." << std::endl;

    // ===============================================================
    // animator
    // ===============================================================

    Animator animator;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        lastTick = currentTime;
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTick;

        // cout << deltaTime << endl;
        // cout << controller.interface.mouseDirection << endl;

        /* controls here */     
        controller.mouse_controls(window, deltaTime, !imGuiHovered);       
        playerController.update(controller.interface, deltaTime); // player controller
        camera.moveTo(v(playerController.position));

        /* physics */
        jolt.update(); // physics tick

        shader_jolt.update_ssbo(0); // matrices
        shader_bb.update_ssbo(0);   // vec3 positions
        shader_bb.update_ssbo(1);   // textures indices

        /* Render here */
        glClearColor(0.0f, 0.2f, 0.3f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader_main.draw(camera, bo);       // render only insert into render
        shader_jolt.draw(camera, bo_jolt);  // physics objects
        shader_bb.draw(camera, bo_player);  // billboards

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