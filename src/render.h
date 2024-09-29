#pragma once

#define GLEW_STATIC
#include <glew.h>
#include "glmath.h"

namespace DaSilva{
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

    class RenderPool{
        private:
            uint count = 0; // amount of objects in commands counting instances, increment needs to start at 0 else garbage
            vector<mat4> matrices;
        
        public:
            vector<vertex> vertices;
            vector<uint> indices;
            vector<DrawElementsIndirectCommand> commands;

            uint insert(vector<vertex> v, vector<uint> i, vec3 position);
            uint insert(vector<vertex> v, vector<uint> i, uint instanceCount);
            RenderPool();
            ~RenderPool();

            uint getCount();

            const void* getBufferData();
            uint getBufferSize();
    };

    class BufferObject{
        public:
            uint VAO;
            uint VBO;
            uint EBO;
            uint indirectBuffer;
            uint commandCount;

            BufferObject(RenderPool& renderPool); // short hand version
            BufferObject(vector<vertex>& vertices, uint vertexOffset, vector<uint>& indices, vector<DrawElementsIndirectCommand>& commands);
            
            ~BufferObject();
    };

}