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
    struct shaderTextureParams
    {
        vec4 subRegionStart;
        vec4 subRegionSize;
        vec4 textureSize;
        vec4 textureIndex;
    };

    // all the info for makign texture from texture atlas
    struct texturePrefab // for texture Atlasing
    {
        vec2 position;  // position on the texture / atlas
        vec2 size;      // size of sub texture
        float textureSize; // size of original texture eg 1024 by 1024
        int textureID;   // which texture index / id in the textureArray
    };

    enum buttonState{
        normal, hovered, clicked
    };

    struct button
    {
        int index; // index that it is in the vector<textureParamUI>;
        void (*funcPtr)(); // function pointer
        vec2 postion;
        vec2 size;
        texturePrefab normal;
        texturePrefab hoverd;
        texturePrefab clickd;
        buttonState state = buttonState::normal;
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

        // normal version
        void insert(vec2 position, vec2 size, anchor anchor, texturePrefab texture);
        // button overload
        void insert(vec2 position, vec2 size, anchor anchor, texturePrefab texture, 
            texturePrefab hover, texturePrefab click, void (*OnClick)());

        bool bBindlessSupport = false; // flag for toggling texture array vs bindless textures
        
        shaderTextureParams texturePacker(texturePrefab input);
    private:
        Shader uiShader;
        Billboard uiBillboard;

        RenderPool uiRenderPool;
        BufferObject uiBufferObject;
        
        Texture textures; // user interface controls its own textures
        
        controllerI *controller_interface; // pointer to controller interface

        vector<mat4> positions; // billboard positions for ui elements      // for vertex shader
        
        int textureParamsCount = 0;
        vector<shaderTextureParams> textureParams; // billboard positions for ui elements  // for fragment shader

        vector<button> buttons;
    public:
        void update();
        void draw();
};

class MainUI : public UserInterface
{   
    public: MainUI(const char* vertexPath, const char* fragmentPath, bool bindlessSupportFlag, controllerI* controllerInterface);
};

void button1();