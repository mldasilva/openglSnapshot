#version 460 core

// //ssbo
layout(std430, binding = 1) readonly buffer InstanceMatricies {
    mat4 instancedMatrix[];
};
// layout(std430, binding = 1) buffer ObjectIDS {
//     uint objectID[];
// };

// inputs
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Color;
layout(location = 2) in vec2 TexCoord;      

uniform mat4 u_view;
uniform mat4 u_projection;

// outputs
layout(location = 0) out vec2 out_texCoord;
layout(location = 1) out flat uint out_textureID;

void main()
{
    uint id = gl_InstanceID + gl_BaseInstance;
    // vec4 worldPosition =  instancedMatrix[objectID[id]] * vec4(Position, 1.0);
    vec4 worldPosition =  instancedMatrix[id] * vec4(Position, 1.0);

    gl_Position = u_projection * u_view * worldPosition;
    
    out_texCoord    = TexCoord;

    // not so much a matrixIndices more of and object ID
    // out_textureID   = objectID[id]; 

};