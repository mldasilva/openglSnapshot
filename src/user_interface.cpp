#include "user_interface.h"

void UserInterface::init()
{
    // make buffer object from render pool
    uiBufferObject.init(uiRenderPool);

    if(!bBindlessSupport)
    {
        // for texture array support
        string paths[3] = { texture_anim_00, texture_greentop, texture_redtop }; // for texture array support
        textures.loadTextureArray(paths);

        // shader settings and ssbos
        uiShader.set_uniform_location("u_textureArray");
    }

    // bind on 10 binding
    uiShader.create_ssbo(10, positions.size() * sizeof(mat4), positions.data());
    // bind on 10 binding
    uiShader.create_ssbo(11, textureIndices.size() * sizeof(textureParamUI), textureIndices.data());
}

// void UserInterface::insert(vec2 position, vec2 scale)

/// @brief 
/// @param position 
/// @param width 
/// @param height 
/// @param anchor 
/// @param textureStart 
/// @param textureSubSize 
/// @param textureIndex 
/// @param textureSize 
void UserInterface::insert(vec2 position, float width, float height, anchor anchor, vec2 textureStart, vec2 textureSubSize, int textureIndex, int textureSize)
{   
    uiRenderPool.insert(uiBillboard.vertices, uiBillboard.indices, 1);
    
    vec2 anchoredPosition;

    switch(anchor)
    {
        case anchor::topleft:
            anchoredPosition = vec2(position.x + (width / 2), position.y + (height / 2));
            break;
        case anchor::topright:
            anchoredPosition = vec2(position.x - (width / 2), position.y + (height / 2));
            break;
        case anchor::botleft:
            anchoredPosition = vec2(position.x + (width / 2), position.y - (height / 2));
            break;
        case anchor::botright:
            anchoredPosition = vec2(position.x - (width / 2), position.y - (height / 2));
            break;
        default: // center
            anchoredPosition = position;
            break;
    }

    // note! : Normalized Device Coordinates (NDC): In this space, coordinates range from -1 to 1. 
    // The center of the screen is (0, 0), the left edge is -1, the right edge is 1, the top edge is 1, 
    // and the bottom edge is -1.

    // Convert to NDC

    float ndcX = (2.0f * anchoredPosition.x) / controller_interface->windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * anchoredPosition.y) / controller_interface->windowHeight;

    vec2 scale = vec2(width/controller_interface->windowWidth, height/controller_interface->windowHeight);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(scale.x, scale.y, 1.0f)); 
    trans = glm::translate(trans, glm::vec3(ndcX / scale.x, ndcY / scale.y, 0.0f));

    positions.push_back(trans);

    // Adjust y-start to make it relative to the bottom-left (OpenGL convention)

    vec2 adjustedStart = vec2(
        textureStart.x, 
        textureSize - (textureStart.y + textureSubSize.y) // flipping y for textureSize vec2 use .y component
    );
    
    textureParamUI uipackage = {
        vec4(adjustedStart.x, adjustedStart.y,0,0), 
        vec4(textureSubSize.x, textureSubSize.y,0,0), 
        vec4(textureSize, textureSize,0,0),
        vec4(textureIndex,0,0,0)
    };
    textureIndices.push_back(uipackage);
}

void UserInterface::insert(void (*OnClick)(), vec2 position, float width, float height, anchor anchor, vec2 textureStart, vec2 textureSubSize, int textureIndex, int textureSize)
{
    insert(position, width, height, anchor, textureStart, textureSubSize, textureIndex, textureSize);
    
    button b
    {
        OnClick,
        position,
        vec2(width, height)
    };
    buttons.push_back(b);
}

void UserInterface::draw()
{   
    // todo button logic

    // this login goes thru all the buttons only allowing 1 click on left down
    if(controller_interface->mouseLeftDown)
    {
        for(button btn : buttons)
        {
            vec2 pos = controller_interface->mouseScreenPosition;
            if((!doOnce) && pos.x > btn.postion.x && pos.x < (btn.size.x + btn.postion.x) && pos.y > btn.postion.y && pos.y < (btn.size.y + btn.postion.y) )
            {
                doOnce = true;
                btn.funcPtr();
            }
        }
    }
    // reseting the doOnce flag for button
    if(!controller_interface->mouseLeftDown && doOnce)
    {
        doOnce = false;
    }
    
    //glClear(GL_DEPTH_BUFFER_BIT); // could clear depth buffer or control depth in fs

    // Use the shader program
    glUseProgram(uiShader.getID());

    if(!bBindlessSupport)
    {
        // for texture array supported
        textures.bindTextureArray(0);
        glUniform1i(uiShader.get_uniform_location("u_textureArray"), 0);
    }

    //draw
    glBindVertexArray(uiBufferObject.VAO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, uiBufferObject.indirectBuffer);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, uiBufferObject.commandCount, sizeof(DrawElementsIndirectCommand));
}

///
MainUI::MainUI(const char* vertexPath, const char* fragmentPath, bool bindlessSupportFlag, controllerI* controllerInterface) 
: UserInterface(vertexPath, fragmentPath, controllerInterface)
{   

    bBindlessSupport = bindlessSupportFlag;

    // build the ui / front to back
    insert(button1, vec2(50, 50), 124, 124, anchor::topleft, vec2(0,0), vec2(250,500), 2, 1024);
    insert(vec2(0, 0), 150, 400, anchor::topleft, vec2(0,0), vec2(150,400), 0, 1024);


    init(); // call last after all ui inserts
}

void button1()
{
    cout << " hello iv been clicked" << endl;
}