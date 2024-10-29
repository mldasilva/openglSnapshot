#version 460 core

layout(std430, binding = 99) readonly buffer ssboFS1 {
    int textureParams[];
};

// inputs
layout(location = 0) in vec2 in_texCoord;
layout(location = 1) in flat uint in_instanceID; // = gl_InstanceID + gl_BaseInstance;

out vec4 FragColor;

// The texture array contains all the textures
uniform sampler2DArray u_textureArray;  

void main()
{   
    FragColor = vec4(1,0,0,1);
};