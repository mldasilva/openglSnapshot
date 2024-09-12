add_library(imgui_impl_glfw imgui_impl_glfw.cpp)

target_include_directories(imgui_impl_glfw PUBLIC
    ${CMAKE_SOURCE_DIR}/lib     
    ${CMAKE_SOURCE_DIR}/lib/imgui
)

add_library(imgui_impl_opengl3 imgui_impl_opengl3.cpp)

target_include_directories(imgui_impl_opengl3 PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/imgui
)