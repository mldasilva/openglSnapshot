#include "user_interface.h"

void UserInterface::init()
{
    uiBufferObject.init(uiRenderPool);
    // bind on 10 bnding
    uiShader.create_ssbo(10, positions.size() * sizeof(mat4), positions.data());

}

// void UserInterface::insert(vec2 position, vec2 scale)
void UserInterface::insert(vec2 position, float width, float height, anchor anchor)
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

    float ndcX = (2.0f * anchoredPosition.x) / windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * anchoredPosition.y) / windowHeight;

    vec2 scale = vec2(width/windowWidth, height/windowHeight);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(scale.x, scale.y, 1.0f)); 
    trans = glm::translate(trans, glm::vec3(ndcX / scale.x, ndcY / scale.y, 0.0f));

    positions.push_back(trans);
}

void UserInterface::draw()
{
    //glClear(GL_DEPTH_BUFFER_BIT); // could clear depth buffer or control depth in fs

    // Use the shader program
    glUseProgram(uiShader.getID());

    //draw
    glBindVertexArray(uiBufferObject.VAO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, uiBufferObject.indirectBuffer);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, uiBufferObject.commandCount, sizeof(DrawElementsIndirectCommand));
}

MainUI::MainUI(const char* vertexPath, const char* fragmentPath, float width, float height) 
: UserInterface(vertexPath, fragmentPath)
{
    windowWidth = width;
    windowHeight = height;

    insert(vec2(width, 0), 124, 124, anchor::topright);

    // call last after all ui inserts
    init();
}


