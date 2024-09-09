#ifndef gl_shaders_h
#define gl_shaders_h

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <vector>
#include <string.h>

#include "glmath.h"
#include "camera.h"

const string shaders = "../res/shaders/";

// https://ktstephano.github.io/rendering/opengl/mdi
// Struct for MultiDrawElements
struct DrawElementsIndirectCommand{

    // rp.commands[i].count           = objects.object[i].indexCount;
    // rp.commands[i].instanceCount   = 1; // objects.object[i][i].instanceCount
    // rp.commands[i].firstIndex      = indexOffset;
    // rp.commands[i].baseVertex      = vertexOffset;
    // rp.commands[i].baseInstance    = instanceOffset;

    // indexOffset     += objects.object[i].indexCount;
    // vertexOffset    += objects.object[i].vertexCount;
    // instanceOffset  += 1;

    //For DrawElementsIndirectCommand this is interpreted as the number of indices.
    uint count; 
    
    // Number of instances where 0 effectively disables the draw command. 
    // Setting instances to 0 is useful if you have an initial list of draw commands 
    // and want to disable them by the CPU or GPU during a frustum culling 
    // step for example.
    uint instanceCount;

    //For DrawElementsIndirectCommand this is an index (not byte) offset into the bound element array buffer to start reading index data.
    uint firstIndex;

    // For DrawElementsIndirectCommand this is interpreted as an addition to whatever index is read from the element array buffer.
    uint baseVertex;

    // If using instanced vertex attributes, this allows you to offset where the instanced buffer data is read from. 
    // The formula for the final instanced vertex attrib offset is floor(instance / divisor) + baseInstance. 
    // If you are not using instanced vertex attributes then you can use this member for whatever you want, 
    // for example storing a material index that you will manually read from in the shader.
    uint baseInstance;

    // Optional user-defined data goes here - if nothing, stride is 0

    DrawElementsIndirectCommand(uint _count, uint _instanceCount, uint _firstIndex, uint _baseVertex, uint _baseInstance) 
        : count(_count), instanceCount(_instanceCount), firstIndex(_firstIndex), baseVertex(_baseVertex), baseInstance(_baseInstance)  {}
};
// sizeof(DrawElementsIndirectCommand) == 20

void checkCompileErrors(unsigned int shader, std::string type);

class bufferObject{
public:
    uint VAO;
    uint VBO;
    uint EBO;
    uint indirectBuffer;
    uint commandCount;

    bufferObject(vector<vertex>& vertices, uint vertexOffset, vector<uint>& indices, vector<DrawElementsIndirectCommand>& command);
};

class shader{
public:
    uint id;
    vector<uint> ssbo;
    vector<void*> ssbo_map;

    shader(const char* vertexPath, const char* fragmentPath);
    // ~shader();

    void create_ssbo(uint index, uint size, const void * data);
    void draw(camera camera, bufferObject buffer);
    void triangle_debug();
};

class renderPool{
private:
    uint count = 0; // amount of objects in commands counting instances, increment needs to start at 0 else garbage
public:
    vector<vertex> vertices;
    vector<uint> indices;
    vector<DrawElementsIndirectCommand> commands;
    vector<mat4> matrices;

    void insert(vector<vertex> v, vector<uint> i, vec3 position);


};

#endif