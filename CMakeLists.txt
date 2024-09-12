cmake_minimum_required(VERSION 3.20.0)

project(isometric_plus VERSION 0.1.0 LANGUAGES C CXX)

# Add executable
add_executable(isometric_plus main.cpp)

# Add subdirectory
add_subdirectory(lib/glm)
add_subdirectory(lib/imgui)
add_subdirectory(lib/imgui/backends)
add_subdirectory(lib/stb)
add_subdirectory(src)

# Include directories (adjust if your headers are in a specific subdirectory)
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}/lib 
    ${CMAKE_SOURCE_DIR}/lib/glm
    ${CMAKE_SOURCE_DIR}/lib/imgui
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends
    ${CMAKE_SOURCE_DIR}/lib/json
    ${CMAKE_SOURCE_DIR}/lib/stb
    ${CMAKE_SOURCE_DIR}/src
)

# Add the directory where the .lib file is located
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib)

# -lopengl32
find_package(OpenGL REQUIRED) 

# Link libraries (link glfw3 only once)
target_link_libraries(${PROJECT_NAME} PUBLIC 
    OpenGL::GL 
    glfw3 
    glew32s
    imgui
    imgui_demo
    imgui_draw
    imgui_tables
    imgui_widgets
    imGui_wrapper
    imgui_impl_glfw
    imgui_impl_opengl3
    stb_image
    camera
    controller
    gl_shaders
    model
)


