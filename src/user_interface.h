#pragma once

#include "glmath.h"
#include "gl_shaders.h" // draw
#include "render.h"     // render pools
#include "billboard.h"  // billboard

using namespace DaSilva;

namespace DaSilva{
    enum anchor{
        center, topleft, topright, botleft, botright
    };
}


// note!: force inheritance
class UserInterface
{
    // note!: force inheritance
    protected:
        UserInterface(const char* vertexPath, const char* fragmentPath) 
        : uiShader (vertexPath, fragmentPath), uiBillboard(1){};
        void init();
        // ~UserInterface();
        void insert(vec2 position, float width, float height, anchor anchor);
        
        float windowWidth;
        float windowHeight;

    private:
        Shader uiShader;
        Billboard uiBillboard;

        RenderPool uiRenderPool;
        BufferObject uiBufferObject;

        vector<mat4> positions; // billboard positions for ui elements

    public:
        void draw();

};

class MainUI : public UserInterface
{
    private:

    public:
        MainUI(const char* vertexPath, const char* fragmentPath, float width, float height);
};