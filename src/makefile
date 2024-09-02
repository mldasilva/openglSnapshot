GLFW = c:/glfw-3.3.9.bin.WIN64/include
GLEW = c:/glew-2.1.0/include
IMGUI = ../lib/imgui
IMGUIB = ../lib/imgui/backends

ifeq ($(OS),Windows_NT)
	GLFWLib = c:/glfw-3.3.9.bin.WIN64/lib-mingw-w64
	GLEWLib = c:/glew-2.1.0/lib/Release/x64
endif

LINK_FLAGS = -lglfw3dll -lglew32 -lopengl32 -Wall #-mwindows
INCLUDES = -I $(GLFW) -I $(GLEW) 
IMGUI_INCLUDES = -I $(IMGUI) -I $(IMGUIB)
LIBRARIES = -L $(GLFWLib) -L $(GLEWLib)

NAME = game.exe
imgui = imgui.o imgui_draw.o imgui_widgets.o imgui_tables.o imgui_impl_glfw.o imgui_impl_opengl3.o imgui_demo.o

run: output
	rm main.o && clear && echo $(OS) && ../bin/$(NAME)

output: main.o $(imgui) imGui_wrapper.o
	g++ $^ -o ../bin/$(NAME) $(LIBRARIES) $(LINK_FLAGS) -g

main.o: main.cpp
	g++ -c $^ $(INCLUDES) $(IMGUI_INCLUDES)

imGui_wrapper.o: imGui_wrapper.cpp
	g++ -c $< -I $(GLFW) $(IMGUI_INCLUDES) -o $@

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
	g++ -c $< $(IMGUI_INCLUDES) -o $@

imgui_impl_opengl3.o: ../lib/imgui/backends/imgui_impl_opengl3.cpp
	g++ -c $< $(IMGUI_INCLUDES) -o $@

# stb_image.o: ../lib/stb_image.c
# 	g++ -c $^ 

clean:
	rm *.o
del: 
	rm ../bin/game.exe
