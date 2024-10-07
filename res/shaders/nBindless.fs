#version 460 core
// non-bindless-texture variant

// inputs
layout(location = 0) in vec2 in_texCoord;
layout(location = 1) in flat uint in_textureID;

out vec4 FragColor;

uniform sampler2D u_textureSampler;

void main()
{
    // FragColor = vec4(1,0,0,1);
    FragColor = texture(u_textureSampler, in_texCoord);
};