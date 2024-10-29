#version 460 core
// vertex shader

layout(std430, binding = 99) readonly buffer ssboVS1 {
    mat4 matrix[];
};

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Color;
layout(location = 2) in vec2 TexCoord;

uniform mat4 u_view;
uniform mat4 u_projection;

layout(location = 0) out vec2 out_texCoord;
layout(location = 1) out flat uint out_id;

void main()
{
    uint id = gl_InstanceID + gl_BaseInstance;
    vec4 worldPosition =  matrix[id] * vec4(Position, 1.0);

    gl_Position = u_projection * u_view * worldPosition;
    
    out_texCoord    = TexCoord;
    out_id          = id;
};