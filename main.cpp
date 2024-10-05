#include "main.h"
// #include "glmath.h"

#include "model.h"
#include "controller.h"
#include "animator.h"
#include "scene.h"
#include "gl_shaders.h"
#include "jolt_wrapper.h"
// #include "imGui_wrapper.h"

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

// idea:
//----------------
// spell system: spell cards some crafted most found like loot
// talent tree is just like pokemon energy cards dictating what card/spells can be used/ equiped
// cards/spells have random attributes like equipment

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
    // imGui_wrapper   imgui(window);                  // after controller

    DaSilva::Shader          shader_main(shader_default_vs, shader_default_fs);
    DaSilva::RenderPool      render_main;

    DaSilva::Shader          shader_jolt(shader_jolt_vs, shader_jolt_fs);
    DaSilva::RenderPool      render_jolt;

    DaSilva::Shader          shader_bilb(shader_bb_vs, shader_bb_fs);
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
    
    for (size_t i = 0; i < 10; i++)
    {
        /* code */
        jolt.create_object(render_jolt, enviroment_dynamic, cube.getInterface(), Vec3((i*2), 5, 0), Quat::sIdentity());
    }
      
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
    textures.loadTexture(texture_anim_00);  // texture index 0
    textures.loadTexture(texture_dice);     // texture index 1

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
    shader_main.    create_ssbo(2, textures.getBufferSize(),    textures.getBufferData());
    BufferObject    bo_main(render_main);

    shader_jolt.    create_ssbo(1, jolt.getBufferSize(),        jolt.getBufferData());
    BufferObject    bo_jolt(render_jolt);

    shader_bilb.    create_ssbo(4, scene.getBufferSize(),       scene.getBufferData());
    shader_bilb.    create_ssbo(7, animator.getBufferSize(),    animator.getBufferData());
    BufferObject    bo_player(render_bilb);

    std::cout << "hello world.." << std::endl;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        lastTick = currentTime;
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTick;

        /* controls here */     
        controller.         mouse_controls(window, deltaTime, !imGuiHovered);       
        playerController.   update(deltaTime); // player controller
        camera.             moveTo(v(playerController.position));
        scene.update(0, v(playerController.position)); // update the first billboard
        
        /* physics */
        jolt.update(); // physics tick
        animator.update(deltaTime);

        shader_jolt.update_ssbo(0); // matrices
        shader_bilb.update_ssbo(0); // vec3 positions
        shader_bilb.update_ssbo(1); // animations indices

        /* Render here */
        glClearColor(0.0f, 0.2f, 0.3f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader_main.draw(camera, bo_main);       // render only insert into render
        shader_jolt.draw(camera, bo_jolt);  // physics objects
        shader_bilb.draw(camera, bo_player);  // billboards

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