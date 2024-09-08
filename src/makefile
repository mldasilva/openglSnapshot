GLFW 	= c:/glfw-3.3.9.bin.WIN64/include
GLEW 	= c:/glew-2.1.0/include
IMGUI 	= ../lib/imgui
IMGUIB 	= ../lib/imgui/backends
JSON 	= ../lib/json
GLM		= ../lib/glm # v 1.0.1
GLMEXT	= ../lib/glm/ext

ifeq ($(OS),Windows_NT)
	GLFWLib = c:/glfw-3.3.9.bin.WIN64/lib-mingw-w64
	GLEWLib = c:/glew-2.1.0/lib/Release/x64
endif

LINK_FLAGS 		= -lglfw3dll -lglew32 -lopengl32 -Wall #-mwindows
INCLUDES 		= -I $(GLFW) -I $(GLEW) 
GLEW_INLCUDES	= -I $(GLEW)
GLFW_INCLUDES	= -I $(GLFW)
IMGUI_INCLUDES 	= -I $(IMGUI) -I $(IMGUIB)
JSON_INCLUDES 	= -I $(JSON)
GLM_INCLUDES	= -I $(GLM) -I $(GLMEXT)
LIBRARIES 		= -L $(GLFWLib) -L $(GLEWLib)

NAME = game.exe
imgui = imgui.o imgui_draw.o imgui_widgets.o imgui_tables.o imgui_impl_glfw.o imgui_impl_opengl3.o imgui_demo.o

run: output
	rm main.o && clear && echo $(OS) && ../bin/$(NAME)

output: main.o model.o gl_shaders.o camera.o controller.o $(imgui) imGui_wrapper.o 
	g++ $^ -o ../bin/$(NAME) $(LIBRARIES) $(LINK_FLAGS) -g

main.o: main.cpp
	g++ -c $^ $(INCLUDES) $(IMGUI_INCLUDES) $(JSON_INCLUDES) $(GLM_INCLUDES)

model.o: model.cpp
	g++ -c $< $(JSON_INCLUDES) $(GLM_INCLUDES) -o $@

gl_shaders.o: gl_shaders.cpp 
	g++ -c $< $(GLEW_INLCUDES) $(GLM_INCLUDES) -o $@

camera.o: camera.cpp
	g++ -c $< $(GLM_INCLUDES) -o $@

controller.o : controller.cpp
	g++ -c $< $(GLFW_INCLUDES) $(GLM_INCLUDES) -o $@




# imgui
imGui_wrapper.o: imGui_wrapper.cpp
	g++ -c $< $(IMGUI_INCLUDES) $(GLFW_INCLUDES) -o $@

imgui.o: ../lib/imgui/imgui.cpp
	g++ -c $< $(IMGUI_INCLUDES) -o $@

imgui_demo.o: ../lib/imgui/imgui_demo.cpp
	g++ -c $< $(IMGUI_INCLUDES) -o $@

imgui_draw.o: ../lib/imgui/imgui_draw.cpp
	g++ -c $< $(IMGUI_INCLUDES) -o $@

imgui_widgets.o: ../lib/imgui/imgui_widgets.cpp
	g++ -c $< $(IMGUI_INCLUDES) -o $@

imgui_tables.o: ../lib/imgui/imgui_tables.cpp
	g++ -c $< $(IMGUI_INCLUDES) -o $@

imgui_impl_glfw.o: ../lib/imgui/backends/imgui_impl_glfw.cpp
	g++ -c $< $(IMGUI_INCLUDES) $(GLFW_INCLUDES) -o $@

imgui_impl_opengl3.o: ../lib/imgui/backends/imgui_impl_opengl3.cpp
	g++ -c $< $(IMGUI_INCLUDES) -o $@



clean:
	rm *.o
del: 
	rm ../bin/game.exe
