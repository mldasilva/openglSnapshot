#include "user_interface.h"

void UserInterface::init()
{
    // make buffer object from render pool
    uiBufferObject.init(uiRenderPool, GL_DYNAMIC_DRAW);

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
    uiShader.create_ssbo(11, texParams.size() * sizeof(shaderTexParamsStruct), texParams.data());
}

mat4 UserInterface::calcPosition(vec2 position, vec2 size, anchorEnum anchor)
{
    vec2 anchoredPosition;

    switch(anchor)
    {
        case anchorEnum::topleft:
            anchoredPosition = vec2(position.x + (size.x / 2), position.y + (size.y / 2));
            break;
        case anchorEnum::topright:
            anchoredPosition = vec2(position.x - (size.x / 2), position.y + (size.y / 2));
            break;
        case anchorEnum::botleft:
            anchoredPosition = vec2(position.x + (size.x / 2), position.y - (size.y / 2));
            break;
        case anchorEnum::botright:
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

    return trans;
}

void UserInterface::setPosition(int index, vec2 position, vec2 size, anchorEnum anchor)
{
    positions[index] = calcPosition(position, size, anchor);
    isDirty_positions = true;
}

bool UserInterface::mouseCollision(vec2 mouse, vec2 objPosition, vec2 objSize)
{
    return (mouse.x > objPosition.x && mouse.x < (objSize.x + objPosition.x) 
            && mouse.y > objPosition.y && mouse.y < (objSize.y + objPosition.y));
}

vec2 UserInterface::snapToGrid(vec2 position, float gridSize)
{
    return glm::vec2(
        std::round(position.x / gridSize) * gridSize,
        std::round(position.y / gridSize) * gridSize
    );
}

void UserInterface::setProgressBar(int index, float percentage)
{
    bars[index].percent = percentage;
    bars[index].dirtyFlag = true;
}

/// @brief 
/// @param index 
/// @param percentChange 0-100 using (-) subtract from percentage
void UserInterface::modProgressBar(int index, float percentChange)
{
    bars[index].percent = bars[index].percent + percentChange;
    bars[index].dirtyFlag = true;
}

void UserInterface::setTexParams(int index, texturePrefabStruct input)
{
    texParams[index] = texturePacker(input);
    isDirty_textureParams = true;
}

void UserInterface::insert(vec2 position, vec2 size, anchorEnum anchor, texturePrefabStruct texture)
{
    uiRenderPool.insert(uiBillboard.vertices, uiBillboard.indices, 1);
    
    positions.push_back(calcPosition(position, size, anchor));
  
    texParams.push_back(texturePacker(texture));
    texParamsCount++; // used for getting indexs to textureParams and positions
}

// progress bar version
void UserInterface::insert(vec2 position, vec2 size, anchorEnum anchor, texturePrefabStruct texture, float percent, float lerpSpeed)
{
    insert(position, size, anchor, texture);

    progressBarStruct b
    {
        texParamsCount - 1, // index, -1 because array start 0 and insert is before index assignment
        percent,
        percent,
        lerpSpeed,
        position,
        size,
        size,
        anchor,
        texture,
        texture.size,
        false
    };
    bars.push_back(b);
}

// draggables / buttons
void UserInterface::insert(vec2 position, vec2 size, anchorEnum anchor, texturePrefabStruct texture, function<void()> onClick, function<void()> onHover, function<void()> onLeave)
{
    insert(position, size, anchor, texture); // insert into render

    _UIButtons.add(position, size, onClick, onHover, onLeave);
}

/// @brief 
/// @param input texture prefab
/// @return texture prefab information packed into a 16 byte vec4x4 struct easly parsed by opengl shaders
shaderTexParamsStruct UserInterface::texturePacker(texturePrefabStruct input)
{
    vec2 adjustedStart = vec2(
        input.position.x, 
        input.textureSize - (input.position.y + input.size.y) // flipping y for textureSize vec2 use .y component
    );

    // package into size and format the shader can understand and parse
    shaderTexParamsStruct uipackage = {
        vec4(adjustedStart.x, adjustedStart.y,0,0), 
        vec4(input.size.x, input.size.y,0,0), 
        vec4(input.textureSize, input.textureSize,0,0),
        vec4(input.textureID,0,0,0)
    };

    return uipackage;
}

void UserInterface::progressBarsUpdate(float deltaTime)
{
    // bars update
    for(progressBarStruct& b : bars)
    {
        if(b.dirtyFlag)
        {
            //clamping
            if(b.percent > 100)
            {
                b.percent = 100;
            }

            if(b.percent < 0)
            {
                b.percent = 0;
            }

            b.size_current = b.size_original * vec2(b.percent_lerp/100,1); // update size
            b.texture.size  = b.texture_original_size * vec2(b.percent_lerp/100,1); // update texture size 
            
            // update ssbo data
            positions[b.index] = calcPosition(b.postion, b.size_current, b.anchor);
            texParams[b.index] = texturePacker(b.texture);

            // set dirty flags
            isDirty_positions = true;
            isDirty_textureParams = true;

            if(b.percent == b.percent_lerp)
            {
                b.dirtyFlag = false;
                continue; // exit early
            }

            // direction lerping up or down
            if(b.percent < b.percent_lerp)
            {
                b.percent_lerp = b.percent_lerp - deltaTime * b.lerpSpeed;
            }
            else
            {
                b.percent_lerp = b.percent_lerp + deltaTime * b.lerpSpeed;
            }
            
            //clamp / stop
            if(std::fabs(b.percent - b.percent_lerp) <= 0.1f) // 0.1f allowance
            {
                b.percent_lerp = b.percent;
            }
        }
    }
}

void UserInterface::update(float deltaTime)
{
    // this logic goes thru all the buttons only allowing 1 click on left down
    vec2 mouse = controller_interface->rawMouseScreenPos;
    
    controller_interface->isMouseInUI = true; // remove

    progressBarsUpdate(deltaTime);
    
    if(!dragging)
    {
        _UIButtons.update(mouse, controller_interface->mouseLeftDown);
    }

    // special dragging logic
    if(dragging)
    {
        
        // cout << "dragging" << mouse << endl;
        setPosition(0, mouse, vec2(64,64), anchorEnum::center);
        if(!controller_interface->mouseLeftDown)
        {
            dragging = false;
            vec2 snap = snapToGrid(mouse, grid);
            setPosition(0, snap, vec2(64,64), anchorEnum::center); // dragger
            setPosition(draggingIndex, snap, vec2(64,64), anchorEnum::center); // button
            _UIButtons.move(1, snap - (vec2(64,64) * 0.5f));
        }
    }
}

/// @brief
/// @return true if button is being interacted with
void UserInterface::draw()
{   
    // could clear depth buffer or control depth in fs
    //glClear(GL_DEPTH_BUFFER_BIT); 

    if(isDirty_positions)
    {
        // update positions
        uiShader.update_ssbo(0); 
        isDirty_positions = false;
    }
    if(isDirty_textureParams)
    {
        // update textureParams
        uiShader.update_ssbo(1); 
        isDirty_textureParams = false;
    }
    
    // Use the shader program
    uiShader.use();

    if(!bBindlessSupport)
    {
        // for texture array supported
        textures.bindTextureArray(0); // 0 is the slot
        glUniform1i(uiShader.get_uniform_location("u_textureArray"), 0); // 0 is the slot
    }

    //draw
    uiBufferObject.draw();
}
///
///=================================================================================
///
///=================================================================================

UIManager::UIManager(const char* vertexPath, const char* fragmentPath, bool bindlessSupportFlag, controllerI* controllerInterface) 
: UserInterface(vertexPath, fragmentPath, controllerInterface)
{   
    bBindlessSupport = bindlessSupportFlag;
    
    // examle for prefabs for different button states
    texturePrefabStruct btn1_prefab = {vec2(0,0), vec2(224,113), 1024, 0};
    texturePrefabStruct btn1_hoverd = {vec2(224,0), vec2(224,113), 1024, 0};
    texturePrefabStruct btn1_clickd = {vec2(524,0), vec2(224,113), 1024, 0};

    // build the ui / front to back 
    // index 0 perserved for dragging icon tooltip
    insert(vec2(280, 320), vec2(64, 64), anchorEnum::topleft, btn1_prefab);

    // progress bar
    insert(vec2(50, 10), vec2(200, 80), anchorEnum::topleft, btn1_prefab, 100.0f, 50.0f);
  
    // button
    insert(vec2(280, 120), vec2(200, 80), anchorEnum::topleft, btn1_prefab, 
    [this, btn1_clickd]() 
    { 
        this->modProgressBar(0, -60); // onclick
        this->setTexParams(2, btn1_clickd);
    }, 
    [this, btn1_hoverd]()
    {
        // on enter
        this->setTexParams(2, btn1_hoverd);
    
    }, 
    [this, btn1_prefab]()
    {
        // on enter
        this->setTexParams(2, btn1_prefab);
    
    });
 
    // draggable
    insert(vec2(280, 320), vec2(64, 64), anchorEnum::topleft, btn1_prefab, 
    [this]() 
    {
        this->dragging = true;
        this->draggingIndex = 3; // cause this is render obj 3
        this->buttonDragID = 1; // cause this is button 1

    }, nullptr, nullptr);

    // lambas examples
    // _UIButtons.add(
    //     vec2(0, 0),
    //     vec2(400, 400),
    //     []() { std::cout << "Button clicked!" << std::endl; }, // onClick lambda
    //     []() { std::cout << "Mouse entered button!" << std::endl; }, // onEnter lambda
    //     []() { std::cout << "Mouse left button!" << std::endl; } // onLeave lambda
    // );

    init(); // call last after all ui inserts

    // UserInterface* ui = static_cast<UserInterface*>(ptr);
}

// InventoryUI::InventoryUI(const char *vertexPath, const char *fragmentPath, bool bindlessSupportFlag, controllerI *controllerInterface)
// : UserInterface(vertexPath, fragmentPath, controllerInterface)
// {
//     // todo
//     bBindlessSupport = bindlessSupportFlag;
    
//     // examle for prefabs for different button states
//     texturePrefabStruct btn1_prefab = {vec2(0,0), vec2(224,113), 1024, 0};
//     texturePrefabStruct btn1_hoverd = {vec2(224,0), vec2(224,113), 1024, 0};
//     texturePrefabStruct btn1_clickd = {vec2(524,0), vec2(224,113), 1024, 0};

//     // build the ui / front to back 
//     // index 0 perserved for dragging icon tooltip
//     insert(vec2(280, 320), vec2(64, 64), anchorEnum::topleft, btn1_prefab);

//     init(); // call last after all ui inserts
// }

// UIManager::UIManager(bool bindlessSupport, controllerI *controllerInterface)
// {
//     // create main ui
//     // create on heap
//     pUIs.push_back(new MainUI(UINB_vs, UINB_fs, bindlessSupport, controllerInterface));
//     // pUIs.push_back(InventoryUI(UINB_vs, UINB_fs, bindlessSupport, controllerInterface));

// }
// UIManager::~UIManager()
// {
//     // for(UserInterface p : pUIs)
//     // {
//     //     // delete(p);
//     // }
// }
// void UIManager::update(float deltaTime)
// {
//     for(UserInterface* p : pUIs)
//     {
//         p->update(deltaTime);
//     }
// }

// void UIManager::draw()
// {
//     for(UserInterface *p : pUIs)
//     {
//         p->draw();
//     }
// }


