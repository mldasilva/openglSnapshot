#include "main.h"
// #include "glmath.h"

#include "model.h"
#include "controller.h"
#include "animator.h"
#include "scene.h"
#include "gl_shaders.h"
#include "jolt_wrapper.h"
#include "wfc.h"
// #include "imGui_wrapper.h"
#include <vector>
//big things:
// ---------------
// lights
// shadows
// wfc - wave function collapse

//smaller things:
//------------------
// skills, abilities
// quest
// ai
// combat

// huge things:
//------------------
// networking

// idea:
//----------------
// spell system: spell cards some crafted most found like loot
// talent tree is just like pokemon energy cards dictating what card/spells can be used/ equiped
// cards/spells have random attributes like equipment

// know issue:
// 1) dragging window freezes game and messes up deltatime
// 2) multiple different functions to open files to strings
// 3) wayland on linux nvidia card "failed to load plugin" libdecor-gtk.so: failed to init"
// https://stackoverflow.com/questions/77739428/how-to-compile-an-opengl-project-in-cmake-in-wayland-desktop-environment
// echo $XDG_SESSION_TYPE
// 4) player stuttering on low end machine, especially when youtube is running or other high required programs
// maybe the player physics scene position get out of sync
// Try decoupling your physics updates from rendering (fixed timestep for physics).
// 
int main(void)
{
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfw_hints();

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 720, "Hello World", nullptr, nullptr);
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

    bool bindless_supported = false;
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // Enable vsync

    /*m: init glew required to be after glfwMakeContextCurrent()
    glew site: First you need to create a valid OpenGL rendering context and call glewInit() to initialize the extension entry points*/ 
    if (GLEW_OK != glewInit()) return -1;

    init_glfw_debugger(window);


    WfcTiled world(120,90);
    // world.printIndices();
    world.generate();

    world.printValues();

    

    return 0;

    // Check if bindless textures are supported
    if (glewIsSupported("GL_ARB_bindless_texture") && glewIsSupported("GL_ARB_gpu_shader_int64")) {
        std::cout << "Bindless textures are supported on this system!" << std::endl;
        bindless_supported = true;
    } else {
        std::cout << "Bindless textures are NOT supported on this system." << std::endl;
        bindless_supported = false;
    }

    // ===============================================================
    // main
    // ===============================================================

    Camera          camera(width, height);
    Controller      controller(window, &camera);    // after camera
    // imGui_wrapper   imgui(window);               // after controller

    string fs1 = string(shader_default_fs);
    string fs2 = string(shader_jolt_fs);
    string fs3 = string(shader_bb_fs);

    if(!bindless_supported)
    {
        fs1 = string(shader_nBindless_fs);
        fs2 = string(shader_nBindless_fs);
        fs3 = string(shader_nBindless_fs);
    }

    DaSilva::Shader          shader_main(shader_default_vs, fs1.c_str());
    DaSilva::RenderPool      render_main;

    DaSilva::Shader          shader_jolt(shader_jolt_vs, fs2.c_str());
    DaSilva::RenderPool      render_jolt;

    DaSilva::Shader          shader_bilb(shader_bb_vs, fs3.c_str());
    DaSilva::RenderPool      render_bilb;

    JoltWrapper     jolt; // Now we can create the actual physics system.

    // ===============================================================
    // model loading
    // ===============================================================

    Model cube(model_cube);
    Model cone(model_cone);
    
    // ===============================================================
    // test world scene
    // ===============================================================

    render_main.insert(cone.vertices, cone.indices, vec3(0, -1, 0));
    
    // for (size_t i = 0; i < 10; i++)
    // {
    //     /* code */
    //     jolt.create_object(render_jolt, enviroment_dynamic, cube.getInterface(), Vec3((i*2), 5, 0), Quat::sIdentity());
    // }
      
    // floor
    BodyID floor = jolt.create_shape(new BoxShape(Vec3(100.0f, 1.0f, 100.0f)), false, Vec3(0.0, -1.0, 0.0));

    jolt.create_object(render_jolt, enviroment_static, cube.getInterface(), Vec3(3, 1, 3), Quat::sIdentity());
    jolt.create_object(render_jolt, enviroment_static, cube.getInterface(), Vec3(0.5f, 1, 3), Quat::sIdentity());
    
    // add floor to joly body filter
    MyBodyFilter floorFilter(floor);
    jolt.pBodyFilter = &floorFilter;
    
    // ===============================================================
    // textures
    // ===============================================================
    
    DaSilva::Texture textures;
    if(bindless_supported)
    {
        textures.loadTexture(texture_anim_00);  // texture index 0
        textures.loadTexture(texture_dice);     // texture index 1
    }
    else
    {
        textures.loadTexture(texture_anim_00, 0);  // texture index 0
        textures.loadTexture(texture_dice, 1);     // texture index 1
    }

    // ===============================================================
    // blending settings
    // ===============================================================
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // ===============================================================
    // animator & billboard scene & player controller
    // ===============================================================
    
    Animator animator(3);
    animator.setAnimation("null", 0);
    animator.setAnimation("hello", 1);
    animator.setAnimation("hello1", 2);
   
    Scene scene;
    scene.add("player1", vec3(-3.0f,10.0f,0.0f));
    scene.add("enemy01", vec3(-3.0f,10.0f,5.0f));
    scene.add("enemy02", vec3(15.0f,10.0f,0.0f));
    
    Billboard billboard(1);
    render_bilb.insert(billboard.vertices, billboard.indices, 10);

    // create player controller
    PlayerController playerController(
        &jolt, &animator, 
        &controller.interface, &camera.interface,
        v(scene.fetch("player1")));
    
    // ===============================================================
    // finalizing before scene
    // ===============================================================
    // jolt optimize shader ssbo creation and bufferObject must be after object creation,
    
	jolt.optimize();

    shader_main.    create_ssbo(0, render_main.getBufferSize(), render_main.getBufferData());
    if(bindless_supported)
    {
        shader_main.create_ssbo(2, textures.getBufferSize(), textures.getBufferData());
    }
    else
    {
        shader_main.set_uniform_location("u_textureSampler");
        shader_jolt.set_uniform_location("u_textureSampler");
        shader_bilb.set_uniform_location("u_textureSampler");
        cout << " set up texture sampler" << endl;
    }
    BufferObject    bo_main(render_main);

    shader_jolt.    create_ssbo(1, jolt.getBufferSize(),        jolt.getBufferData());
    BufferObject    bo_jolt(render_jolt);

    shader_bilb.    create_ssbo(4, scene.getBufferSize(),       scene.getBufferData());
    shader_bilb.    create_ssbo(7, animator.getBufferSize(),    animator.getBufferData());
    BufferObject    bo_player(render_bilb);

    std::cout << "hello world.." << std::endl;

    // return -1;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        lastTick = currentTime;
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTick;
        // deltaTime = fmax(currentTime - lastTick, 0.001f);

        controller.         mouse_controls(window, deltaTime, !imGuiHovered);       
        playerController.   update(deltaTime);     // Update player first
        scene.              update(0, v(playerController.position)); // Update billboard
        camera.             moveTo(v(playerController.position));   // Move camera to player's new position

        // cout << deltaTime << endl;
        /* physics */
        jolt.update(deltaTime); // physics tick
        animator.update(deltaTime);

        shader_jolt.update_ssbo(0); // matrices
        shader_bilb.update_ssbo(0); // vec3 positions
        shader_bilb.update_ssbo(1); // animations indices

        /* Render here */
        glClearColor(0.0f, 0.2f, 0.3f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader_main.draw(camera, bo_main);      // render only insert into render
        shader_jolt.draw(camera, bo_jolt);      // physics objects
        shader_bilb.draw(camera, bo_player);    // billboards

        // imgui.start_frame();
        // imgui.demo(&imGuiHovered);
        // imgui.rendering();

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