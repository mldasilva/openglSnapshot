# ==================================================
# animator
# ==================================================
add_library(animator STATIC animator.cpp)

target_include_directories(animator PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/glm
)

# ==================================================
# camera
# ==================================================
add_library(camera STATIC camera.cpp)

target_include_directories(camera PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/glm
)

# ==================================================
# controller
# ==================================================

add_library(controller STATIC controller.cpp)

target_include_directories(controller PUBLIC 
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/lib/glm
)

# ==================================================
# render
# ==================================================

add_library(render STATIC render.cpp)

target_include_directories(render PUBLIC
    ${CMAKE_SOURCE_DIR}/lib    
    ${CMAKE_SOURCE_DIR}/lib/glm
)

# ==================================================
# gl_shaders
# ==================================================

add_library(gl_shaders STATIC gl_shaders.cpp)

target_include_directories(gl_shaders PUBLIC 
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/lib/glm
    ${CMAKE_SOURCE_DIR}/lib/stb
)

# ==================================================
# imGui_wrapper
# ==================================================

# add_library(imGui_wrapper STATIC imGui_wrapper.cpp)

# target_include_directories(imGui_wrapper PUBLIC 
#     ${CMAKE_SOURCE_DIR}/lib
#     ${CMAKE_SOURCE_DIR}/lib/imgui
#     ${CMAKE_SOURCE_DIR}/lib/imgui/backends
# )

# ==================================================
# model
# ==================================================

add_library(model STATIC model.cpp)

target_include_directories(model PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/glm
    ${CMAKE_SOURCE_DIR}/lib/json
)

# ==================================================
# jolt wrapper
# ==================================================

# include(${CMAKE_SOURCE_DIR}/lib/jolt/Jolt/jolt.cmake)

add_library(jolt_wrapper STATIC jolt_wrapper.cpp)

target_include_directories(jolt_wrapper PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/jolt/
    ${CMAKE_SOURCE_DIR}/lib/glm
    ${CMAKE_SOURCE_DIR}/lib
)

# have to link it if using compiled library
target_link_libraries(jolt_wrapper PRIVATE Jolt)  # Link against the Jolt Physics shared library

# ==================================================
# wfc
# ==================================================

add_library(wfc STATIC wfc.cpp)

target_include_directories(wfc PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/glm
)
