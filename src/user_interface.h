#pragma once

#include "glmath.h"
#include "gl_shaders.h" // draw
#include "render.h"     // render pools
#include "billboard.h"  // billboard
#include "assets.h"
#include "controller_interface.h"
#include "user_interface/UIButtons.h"
#include <functional>

using namespace DaSilva;

namespace DaSilva{
    enum anchorEnum{
        center, topleft, topright, botleft, botright
    };

    // vec4 for opengl std430 padding ease since i couldnt figure out the padding
    struct shaderTexParamsStruct
    {
        vec4 subRegionStart;
        vec4 subRegionSize;
        vec4 textureSize;
        vec4 textureIndex;
    };

    // all the info for making texture from texture atlas
    struct texturePrefabStruct // for texture Atlasing
    {
        vec2 position;  // position on the texture / atlas
        vec2 size;      // size of sub texture
        float textureSize; // size of original texture eg 1024 by 1024
        int textureID;   // which texture index / id in the textureArray
    };

    struct progressBarStruct
    {
        int index;
        float percent;
        float percent_lerp; // use for temp to lerp
        float lerpSpeed;
        vec2 postion; // hitbox
        vec2 size_current;
        const vec2 size_original;
        anchorEnum anchor;
        texturePrefabStruct texture;
        const vec2 texture_original_size;
        bool dirtyFlag; // if requires update
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
        void insert(vec2 position, vec2 size, anchorEnum anchor, texturePrefabStruct texture);

        // progress bar overload
        void insert(vec2 position, vec2 size, anchorEnum anchor, texturePrefabStruct texture, float percent, float lerpSpeed);
        
        // draggable / buttons overload
        void insert(vec2 position, vec2 size, anchorEnum anchor, texturePrefabStruct texture, function<void()> onClick, function<void()> onHover, function<void()> onLeave);

        bool bBindlessSupport = false; // flag for toggling texture array vs bindless textures
        
        shaderTexParamsStruct texturePacker(texturePrefabStruct input);

        bool isDirty_positions = false;      // set true is ssbo requires updating
        bool isDirty_textureParams = false;  // set true is ssbo requires updating

        UIButtons _UIButtons;
    private:
        // helper functions
        mat4 calcPosition(vec2 position, vec2 size, anchorEnum anchor);
        void setPosition(int index, vec2 position, vec2 size, anchorEnum anchor);
        bool mouseCollision(vec2 mouse, vec2 objPosition, vec2 objSize);

        vec2 snapToGrid(vec2 position, float gridSize);
        // update functions
        void progressBarsUpdate(float deltaTime);

        Shader uiShader;
        Billboard uiBillboard;

        RenderPool uiRenderPool;
        BufferObject uiBufferObject;

        Texture textures;                           // user interface controls its own textures
        
        controllerI *controller_interface;          // pointer to controller interface

        vector<mat4> positions;                     // render positions / billboard positions for ui elements      // for vertex shader
        vector<shaderTexParamsStruct> texParams;    // billboard positions for ui elements  // for fragment shader
        int texParamsCount = 0;
        
        // ui elements
        vector<progressBarStruct> bars;
        
    public:
        // draggerStruct dragger; // the dragging icon
        bool dragging = false;
        int draggingIndex = -1;
        int buttonDragID = -1;
        float grid = 64;

        void setProgressBar (int index, float percentage);
        void modProgressBar (int index, float percentChange);
        void setTexParams   (int index, texturePrefabStruct input);

        void update(float deltaTime);
        void draw();
};

class UIManager : public UserInterface
{   
    public: 
        UIManager(const char* vertexPath, const char* fragmentPath, bool bindlessSupportFlag, controllerI* controllerInterface);
};

// class InventoryUI : public UserInterface
// {   
//     public: 
//         InventoryUI(const char* vertexPath, const char* fragmentPath, bool bindlessSupportFlag, controllerI* controllerInterface);
// };

// class UIManager
// {
//     private:
//         vector<UserInterface*> pUIs;

//     public:
//         UIManager(bool bindlessSupport, controllerI* controllerInterface);
//         ~UIManager();
//         void update(float deltaTime);
//         void draw();
// };