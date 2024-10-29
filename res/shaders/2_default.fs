#version 460 core
// non-bindless-texture variant fragment shader

layout(std430, binding = 99) readonly buffer ssboFS1 {
    int textureParams[];
};

layout(location = 0) in vec2 in_texCoord;
layout(location = 1) in flat uint in_id;

out vec4 FragColor;

// The texture array contains all the textures
uniform sampler2DArray u_textureArray;  

void main()
{
    FragColor = texture(u_textureArray, vec3(in_texCoord, textureParams[in_id]));
};