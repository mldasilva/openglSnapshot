#include "render.h"

// =======================================================================================
//
// Buffer Object
//
// =======================================================================================

DaSilva::BufferObject::BufferObject()
{
    // cout << "creating buffer object..." << endl;
}

void DaSilva::BufferObject::init(RenderPool &renderPool)
{
    // BufferObject(renderPool.vertices, 8, renderPool.indices, renderPool.commands);
    
    glGenVertexArrays(1, &VAO); 
    glBindVertexArray(VAO); 
    
    glGenBuffers(1, &VBO); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * renderPool.vertices.size(), renderPool.vertices.data(), GL_STATIC_DRAW); 
    

    // assuming 8 vertex offset
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texUV));


    glGenBuffers(1, &EBO); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * renderPool.indices.size(), renderPool.indices.data(), GL_STATIC_DRAW); 
    

    if(renderPool.commands.data() != NULL)
    {
        glGenBuffers(1, &indirectBuffer); 
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer); 
        glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand) * renderPool.commands.size(), renderPool.commands.data(), GL_STATIC_DRAW); // GL_STATIC_DRAW
        // glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        commandCount = renderPool.commands.size();
    }
    else{
        // printf("bufferObject command error\n");
        cout << "bufferObject command error\n" << endl;
        return;
    }
}

// dynamic indirect buffer required for indirect moding
void DaSilva::BufferObject::init(RenderPool &renderPool, uint indirectDrawType)
{
    // BufferObject(renderPool.vertices, 8, renderPool.indices, renderPool.commands);
    
    glGenVertexArrays(1, &VAO); 
    glBindVertexArray(VAO); 
    
    glGenBuffers(1, &VBO); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * renderPool.vertices.size(), renderPool.vertices.data(), GL_STATIC_DRAW); 
    

    // assuming 8 vertex offset
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texUV));


    glGenBuffers(1, &EBO); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * renderPool.indices.size(), renderPool.indices.data(), GL_STATIC_DRAW); 
    

    if(renderPool.commands.data() != NULL)
    {
        glGenBuffers(1, &indirectBuffer); 
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer); 
        glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand) * renderPool.commands.size(), renderPool.commands.data(), indirectDrawType); // GL_STATIC_DRAW
        // glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        commandCount = renderPool.commands.size();
    }
    else{
        // printf("bufferObject command error\n");
        cout << "bufferObject command error\n" << endl;
        return;
    }
}

void DaSilva::BufferObject::init(vector<vertex>& vertices, uint vertexOffset, vector<uint>& indices, vector<DrawElementsIndirectCommand>& commands)
{
    // cout << "creating buffer object..." << endl;
    glGenVertexArrays(1, &VAO); 
    glBindVertexArray(VAO); 
    
    glGenBuffers(1, &VBO); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW); 
    
    if(vertexOffset == 3)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexOffset * sizeof(GLfloat), (GLvoid*)0);
    }
    else if(vertexOffset == 8)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texUV));
    }
    else{
        // printf("bufferObject error\n");
        cout << "bufferObject error\n" << endl;
        return;
    }

    glGenBuffers(1, &EBO); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), indices.data(), GL_STATIC_DRAW); 
    

    if(commands.data() != NULL)
    {
        glGenBuffers(1, &indirectBuffer); 
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer); 
        glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand) * commands.size(), commands.data(), GL_STATIC_DRAW);
        // glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        commandCount = commands.size();
    }
    else{
        // printf("bufferObject command error\n");
        cout << "bufferObject command error\n" << endl;
        return;
    }

}

// not 100% related to this class but useful to keep together
// this way has some disadvantaged vs glmaprange

// glBufferSubData

//     Usage: Uploads a specified range of data to a buffer that is already bound.
//     How It Works: Directly writes data to a buffer on the GPU from the CPU without needing to map the buffer into CPU memory.

// Pros:

//     Simple to use for small, infrequent updates.
//     No need to map/unmap the buffer, reducing potential errors.
//     The OpenGL driver handles synchronization and ensures that the buffer is not in use before modifying the data.

// Cons:

//     Can be inefficient for frequent updates, especially if youre modifying small parts of a large buffer. Each call might introduce overhead.
//     Generally slower than mapped buffer access when making multiple updates, as each call to glBufferSubData may involve additional validation or GPU synchronization.

void DaSilva::BufferObject::subData_instanceCount(uint index, uint newValue)
{
    GLintptr instanceCountOffset = offsetof(DrawElementsIndirectCommand, instanceCount) + (sizeof(DrawElementsIndirectCommand) * index);
    glBufferSubData(GL_DRAW_INDIRECT_BUFFER, instanceCountOffset, sizeof(GLuint), &newValue);
}


// glMapBufferRange with memcpy

//     Usage: You map the buffer (using glMapBufferRange or glMapBuffer), then use memcpy to write data into the mapped memory.
//     How It Works: Similar to glMapBufferRange, you first map the buffer and then perform memory operations (like memcpy) directly to modify specific sections.

// Pros:

//     Same benefits as glMapBufferRange: low-latency access, efficient for multiple updates, and direct memory manipulation.
//     memcpy provides a familiar interface for copying larger blocks of memory efficiently.

// Cons:

//     The same complexity and potential synchronization issues as glMapBufferRange.
//     Careful buffer management is required to ensure the GPU doesnt use the buffer while it is mapped.
//     Slightly more manual memory management compared to using glBufferSubData.

// When to Use:

//     When you want to batch-copy data to a buffer or update multiple fields in a structure in one call.
//     If you are comfortable managing mapped buffers and need the performance boost from direct memory access.


void DaSilva::BufferObject::memcpy_instanceCount(uint index, uint newValue)
{
    GLintptr instanceCountOffset = offsetof(DrawElementsIndirectCommand, instanceCount) + (sizeof(DrawElementsIndirectCommand) * index);
    
    // // Map the buffer for writing (optional: GL_MAP_INVALIDATE_BUFFER_BIT can be used if replacing entire content)
    void* bufferPtr = glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, instanceCountOffset, sizeof(GLuint), 
                                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
    if (bufferPtr) 
    {
        // Modify instance count
        memcpy(bufferPtr, &newValue, sizeof(GLuint));

        // Unmap the buffer after modification
        glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
    }
    
}

void DaSilva::BufferObject::draw()
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, commandCount, sizeof(DrawElementsIndirectCommand));
}

// when using local renderpool
const void *DaSilva::BufferObject::getBufferData()
{
    return renderPool.getBufferData();
}

// when using local renderpool
uint DaSilva::BufferObject::getBufferSize()
{
    return renderPool.getBufferSize();
}

DaSilva::BufferObject::~BufferObject()
{
    // cout << "deleting buffer object" << endl;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &indirectBuffer); 
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    // if(indirect_map != nullptr)
    // glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
}

void DaSilva::BufferObject::init()
{
    init(renderPool, GL_DYNAMIC_DRAW);
}

// =======================================================================================
//
// Render Pool
//
// =======================================================================================
DaSilva::RenderPool::RenderPool()
{
    // cout << "creating renderPool object..." << endl;
}

DaSilva::RenderPool::~RenderPool()
{
    // cout << "deleting renderPool object..." << endl;
}

uint DaSilva::RenderPool::getCount()
{
    return count;
}

const void *DaSilva::RenderPool::getBufferData()
{
    return matrices.data();
}

uint DaSilva::RenderPool::getBufferSize()
{
    return matrices.size() * sizeof(mat4);
}

/// @brief 
/// @param v 
/// @param i 
/// @param position 
/// @return renderID / index that model information was inserted
uint DaSilva::RenderPool::insert(vector<vertex> v, vector<uint> i, vec3 position)
{
    // rp.commands[i].count           = objects.object[i].indexCount;
    // rp.commands[i].instanceCount   = 1; // objects.object[i][i].instanceCount
    // rp.commands[i].firstIndex      = indexOffset;
    // rp.commands[i].baseVertex      = vertexOffset;
    // rp.commands[i].baseInstance    = instanceOffset;

    // indexOffset     += objects.object[i].indexCount;
    // vertexOffset    += objects.object[i].vertexCount;
    // instanceOffset  += 1;

    commands.push_back(DrawElementsIndirectCommand(i.size(), 1, indices.size(), vertices.size(), count));
    vertices.insert(vertices.end(), v.begin(), v.end());
    indices.insert(indices.end(), i.begin(), i.end());

    matrices.push_back(translate(mat4(1.0f), position));

    count++;

    return count - 1; // return what index it was inserted // aka render id
}

/// @brief no position data in this one the matrices are empty 
///         and assumed to be provided else where such as a physics engine
/// @param v vertex data
/// @param i index data
/// @return the index of insertsion
uint DaSilva::RenderPool::insert(vector<vertex> v, vector<uint> i, uint instanceCount)
{
    // rp.commands[i].count           = objects.object[i].indexCount;
    // rp.commands[i].instanceCount   = 1; // objects.object[i][i].instanceCount
    // rp.commands[i].firstIndex      = indexOffset;
    // rp.commands[i].baseVertex      = vertexOffset;
    // rp.commands[i].baseInstance    = instanceOffset;

    // indexOffset     += objects.object[i].indexCount;
    // vertexOffset    += objects.object[i].vertexCount;
    // instanceOffset  += 1;

    commands.push_back(DrawElementsIndirectCommand(i.size(), instanceCount, indices.size(), vertices.size(), count));
    vertices.insert(vertices.end(), v.begin(), v.end());
    indices.insert(indices.end(), i.begin(), i.end());

    // matrices.push_back(translate(mat4(1.0f), position));

    count++;

    return count - 1; // return what index it was inserted // aka render id
}