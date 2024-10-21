#pragma once

#include "glmath.h"
#include "gl_shaders.h" // draw
#include "render.h"     // render pools
#include "billboard.h"  // billboard
#include "assets.h"
#include "controller_interface.h"

using namespace DaSilva;

namespace DaSilva{
    enum anchor{
        center, topleft, topright, botleft, botright
    };

    // vec4 for opengl std430 padding ease since i couldnt figure out the padding
    struct textureParamUI
    {
        vec4 subRegionStart;
        vec4 subRegionSize;
        vec4 textureSize;
        vec4 textureIndex;
    };

    struct button
    {
        void (*funcPtr)(); // function pointer
        vec2 postion;
        vec2 size;
    };

}

// note!: force inheritance
class UserInterface
{
    // note!: force inheritance
    protected:
        UserInterface(const char* vertexPath, const char* fragmentPath, controllerI* controllerInterface) 
        : uiShader (vertexPath, fragmentPath), uiBillboard(1){controller_interface = controllerInterface;};
        void init();
        // ~UserInterface();
        void insert(vec2 position, float width, float height, anchor anchor, vec2 textureStart, vec2 textureSubSize, int textureIndex, int textureSize);
        // button overload
        void insert(void (*OnClick)(), vec2 position, float width, float height, anchor anchor, vec2 textureStart, vec2 textureSubSize, int textureIndex, int textureSize);

        bool bBindlessSupport = false; // flag for toggling texture array vs bindless textures
    
    private:
        Shader uiShader;
        Billboard uiBillboard;

        RenderPool uiRenderPool;
        BufferObject uiBufferObject;

        Texture textures; // user interface controls its own textures
        
        controllerI *controller_interface; // pointer to controller interface

        vector<mat4> positions; // billboard positions for ui elements      // for vertex shader
        vector<textureParamUI> textureIndices; // billboard positions for ui elements  // for fragment shader

        bool doOnce = false; // flag for button to only fire once.
        vector<button> buttons;
    public:
        void draw();

};

class MainUI : public UserInterface
{
    private:
        
    public:
        MainUI(const char* vertexPath, const char* fragmentPath, bool bindlessSupportFlag, controllerI* controllerInterface);
};

void button1();