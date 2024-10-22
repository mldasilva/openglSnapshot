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
    uiShader.create_ssbo(11, textureParams.size() * sizeof(shaderTextureParams), textureParams.data());
}

void UserInterface::insert(vec2 position, vec2 size, anchor anchor, texturePrefab texture)
{
    uiRenderPool.insert(uiBillboard.vertices, uiBillboard.indices, 1);
    
    vec2 anchoredPosition;

    switch(anchor)
    {
        case anchor::topleft:
            anchoredPosition = vec2(position.x + (size.x / 2), position.y + (size.y / 2));
            break;
        case anchor::topright:
            anchoredPosition = vec2(position.x - (size.x / 2), position.y + (size.y / 2));
            break;
        case anchor::botleft:
            anchoredPosition = vec2(position.x + (size.x / 2), position.y - (size.y / 2));
            break;
        case anchor::botright:
            anchoredPosition = vec2(position.x - (size.x / 2), position.y - (size.y / 2));
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

    vec2 scale = vec2(size.x/controller_interface->windowWidth, size.y/controller_interface->windowHeight);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(scale.x, scale.y, 1.0f)); 
    trans = glm::translate(trans, glm::vec3(ndcX / scale.x, ndcY / scale.y, 0.0f));

    positions.push_back(trans);
  
    textureParams.push_back(texturePacker(texture));
    textureParamsCount++;
}

void UserInterface::insert(vec2 position, vec2 size, anchor anchor, texturePrefab texture, texturePrefab hover, texturePrefab click, void (*OnClick)())
{
    insert(position, size, anchor, texture);
    
    button b
    {
        textureParamsCount - 1, // index, -1 because array start 0 and insert is before index assignment
        OnClick,
        position,
        size,
        texture,
        hover,
        click,
        buttonState::normal
    };
    buttons.push_back(b);
}

/// @brief 
/// @param input texture prefab
/// @return texture prefab information packed into a 16 byte vec4x4 struct easly parsed by opengl shaders
shaderTextureParams UserInterface::texturePacker(texturePrefab input)
{
    vec2 adjustedStart = vec2(
        input.position.x, 
        input.textureSize - (input.position.y + input.size.y) // flipping y for textureSize vec2 use .y component
    );

    // package into size and format the shader can understand and parse
    shaderTextureParams uipackage = {
        vec4(adjustedStart.x, adjustedStart.y,0,0), 
        vec4(input.size.x, input.size.y,0,0), 
        vec4(input.textureSize, input.textureSize,0,0),
        vec4(input.textureID,0,0,0)
    };

    return uipackage;
}

void UserInterface::update()
{
    // this logic goes thru all the buttons only allowing 1 click on left down
    vec2 pos = controller_interface->rawMouseScreenPos;
    for(button& btn : buttons) // reference to make changes
    {
        // button state control:
        //===================================
        // if we are in a button hit box
        if(pos.x > btn.postion.x && pos.x < (btn.size.x + btn.postion.x) 
        && pos.y > btn.postion.y && pos.y < (btn.size.y + btn.postion.y) )
        {
            controller_interface->isMouseInUI = true;

            // clicked do once / good example of states stopping from if statements being multi fired
            if((btn.state != buttonState::clicked) && controller_interface->mouseLeftDown)
            {
                btn.state = buttonState::clicked;
                cout << "clicked" << endl;
                btn.funcPtr();

                textureParams[btn.index] = texturePacker(btn.clickd);
            }
            // hovered
            if((btn.state != buttonState::hovered) && (!controller_interface->mouseLeftDown))
            {
                btn.state = buttonState::hovered;
                cout << "hovered" << endl;
                textureParams[btn.index] = texturePacker(btn.hoverd);
            }
        }
        else
        {
            // reset
            if(btn.state != buttonState::normal)
            {
                controller_interface->isMouseInUI = false;
                btn.state = buttonState::normal;
                cout << "normal" << endl;
                textureParams[btn.index] = texturePacker(btn.normal);
            }
        }
    }
}

/// @brief
/// @return true if button is being interacted with
void UserInterface::draw()
{   
    // could clear depth buffer or control depth in fs
    //glClear(GL_DEPTH_BUFFER_BIT); 

    // update textureParams
    uiShader.update_ssbo(1); 
    
    // Use the shader program
    glUseProgram(uiShader.getID());

    if(!bBindlessSupport)
    {
        // for texture array supported
        textures.bindTextureArray(0); // 0 is the slot
        glUniform1i(uiShader.get_uniform_location("u_textureArray"), 0); // 0 is the slot
    }

    //draw
    glBindVertexArray(uiBufferObject.VAO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, uiBufferObject.indirectBuffer);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, uiBufferObject.commandCount, sizeof(DrawElementsIndirectCommand));
}
///
///=================================================================================
///
///=================================================================================

MainUI::MainUI(const char* vertexPath, const char* fragmentPath, bool bindlessSupportFlag, controllerI* controllerInterface) 
: UserInterface(vertexPath, fragmentPath, controllerInterface)
{   
    bBindlessSupport = bindlessSupportFlag;
    
    // examle for prefabs for different button states
    texturePrefab btn1_prefab = {vec2(0,0), vec2(224,113), 1024, 0};
    texturePrefab btn1_hoverd = {vec2(224,0), vec2(224,113), 1024, 0};
    texturePrefab btn1_clickd = {vec2(524,0), vec2(224,113), 1024, 0};

    // build the ui / front to back   
    insert(vec2(50, 10), vec2(200, 80), anchor::topleft, btn1_prefab);
    insert(vec2(280, 10), vec2(200, 80), anchor::topleft, btn1_prefab, btn1_hoverd, btn1_clickd, button1);

    init(); // call last after all ui inserts
}

void button1()
{
    cout << " hello iv been clicked" << endl;
}