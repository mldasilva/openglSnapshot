#include "main.h"
// #include "glmath.h"

#include "assets.h"
#include "model.h"
#include "controller.h"
#include "animator.h"
#include "scene.h"
#include "gl_shaders.h"
#include "jolt_wrapper.h"
#include "wfc.h"
#include "billboard.h"
#include "user_interface.h"

#include "shader.h" // v2

// #include "imGui_wrapper.h"
#include <vector>

//big things:
// ---------------
// lights
// shadows
// wfc - wave function collapse
// ui
//  - button
//  - hp bar
//  - drag and drop
//  - hide 
//  - inventory

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
// romulus and remus

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

    // Check if bindless textures are supported
    bindless_supported = (glewIsSupported("GL_ARB_bindless_texture") && glewIsSupported("GL_ARB_gpu_shader_int64"));

    // ===============================================================
    // main
    // ===============================================================
    
    ShaderStorageBufferObject ssbo;
    
    Camera          camera(width, height);
    Controller      controller(window, &camera);    // after camera

    UIManager               userInterface(UINB_vs, UINB_fs, bindless_supported, &controller.interface);
    JoltWrapper     jolt; // Now we can create the actual physics system.


    // ===============================================================
    // model loading
    // ===============================================================

    Model cube(model_cube);
    Model cone(model_cone);
    Billboard billboard(1);

    // ===============================================================
    // test world scene
    // ===============================================================
    #pragma region jolt world

    Shader_2 joltShader(shader_jolt_vs, shader_nBindless_fs);

    RenderPool render_jolt;

    WfcTiled world(15,9);
    world.generate();
    
    for(const auto tile : world.tiles)
    {
        if(tile.tileType == land || tile.tileType == water)
        {
            // render_main.insert(cube.vertices, cube.indices, vec3(tile.col * 2, -1, tile.row * 2));
            jolt.create_object(render_jolt, enviroment_static, cube.getInterface(), Vec3(tile.col * 2, -1, tile.row * 2), Quat::sIdentity());

        }
    }

    // world.printValues();
    // for (size_t i = 0; i < 10; i++)
    // {
    //     /* code */
    //     jolt.create_object(render_jolt, enviroment_dynamic, cube.getInterface(), Vec3((i*2), 5, 0), Quat::sIdentity());
    // }
      
    // floor
    BodyID floor = jolt.create_shape(new BoxShape(Vec3(1.0f, 1.0f, 1.0f)), false, Vec3(0.0, -1.0, 0.0));

    jolt.create_object(render_jolt, enviroment_static, cube.getInterface(), Vec3(3, 1, 3), Quat::sIdentity());
    jolt.create_object(render_jolt, enviroment_static, cube.getInterface(), Vec3(0.5f, 1, 3), Quat::sIdentity());
    
    // add floor to joly body filter
    MyBodyFilter floorFilter(floor);
    jolt.pBodyFilter = &floorFilter;

    BufferObject    bo_jolt; 
    bo_jolt.init(render_jolt);

    #pragma endregion

    // ===============================================================
    // textures
    // ===============================================================
    
    Texture textures;
    textures.loadTextureArray(vector<string>{texture_anim_00, texture_greentop, texture_redtop});
    textures.bindTextureArray(1); // 1 is the slot
    // glUniform1i(mainShader.get_uniform_location("u_textureArray"), 0);
    
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
    scene.add("player1", vec3(5.0f,1.0f,5.0f));
    scene.add("enemy01", vec3(-3.0f,10.0f,5.0f));
    scene.add("enemy02", vec3(15.0f,10.0f,0.0f));
    
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

    // ===============================================================
    // main scene
    // ===============================================================

    // For Textures ssbo
    vector<int> textureIds;
    textureIds.push_back(2);
    textureIds.push_back(0);

    // making bufferObject with all the vertices and indices
    BufferObject bo_main; 
    bo_main.renderPool.insert(cone.getVertices(), cone.getIndices(), vec3(0, 1, 0));
    bo_main.renderPool.insert(cone.getVertices(), cone.getIndices(), vec3(3, 1, 0));
    bo_main.init();

    // making ssbo
    ssbo.add("Render", bo_main.getRenderPoolBufferSize(), bo_main.getRenderPoolBufferData());
    ssbo.add("Textures", textureIds.size() * sizeof(int), textureIds.data());

    // make shader and attach ssbo
    Shader_2 mainShader(v2_default_vs, v2_default_fs);
    mainShader.attachSSBO(ssbo.find("Render"), shaderTypeEnum::vertShader);
    mainShader.attachSSBO(ssbo.find("Textures"), shaderTypeEnum::fragShader);

    mainShader.attachCode(shaderTypeEnum::vertShader);
    mainShader.attachCode(shaderTypeEnum::fragShader);
    mainShader.linkProgram();
    
    // ===============================================================
    // billboards including player
    // ===============================================================
    
    BufferObject bo_player; 
    bo_player.renderPool.insert(billboard.vertices, billboard.indices, 3);
    bo_player.init();

    // using sceene bo_player insert is instance no matrix data
    ssbo.add("BillboardScenePositions", scene.getBufferSize(), scene.getBufferData());

    Shader_2 playerShader(shader_bb_vs, shader_nBindless_fs);
    playerShader.attachSSBO(ssbo.find("BillboardScenePositions"), shaderTypeEnum::vertShader);
    playerShader.attachCode(shaderTypeEnum::vertShader);
    playerShader.attachCode(shaderTypeEnum::fragShader);
    playerShader.linkProgram();


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        lastTick = currentTime;
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTick;
        
        controller.         mouse_controls(window, deltaTime, !imGuiHovered);       
        userInterface.      update(deltaTime);      // update before player controller to stop player movement
        playerController.   update(deltaTime);      // Update player first
        scene.              update(0, v(playerController.position)); // Update billboard
        camera.             moveTo(v(playerController.position));   // Move camera to player's new position

        /* physics */
        jolt.update(deltaTime); // physics tick
        // animator.update(deltaTime);

        ssbo.updateAll();

        /* Render here */
        glClearColor(0.0f, 0.2f, 0.3f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        mainShader.draw(camera, bo_main, 1);  // render only insert into render
        // playerShader.draw(camera, bo_player);    // billboards

        // userInterface.draw();                   // user interface

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        // cout << sizeof(vec3) << endl;
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