#version 460 core
// #extension GL_ARB_bindless_texture : enable
#extension GL_ARB_gpu_shader_int64 : enable

//ssbo input
// layout(std430, binding = 3) readonly buffer TextureIndices {
//     uint textureIndices[];
// };

// layout(std430, binding = 4) readonly buffer textureHandles {
//     uint64_t handles[];
// };

// inputs
layout(location = 0) in vec2 in_texCoord;
layout(location = 1) in flat uint in_textureID;

out vec4 FragColor;

void main()
{
    // FragColor = texture(sampler2D(handles[textureIndices[in_textureID]]), in_texCoord);
    FragColor = vec4(1,0,0,1);
};