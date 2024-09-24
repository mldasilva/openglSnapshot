#version 460 core
// #extension GL_ARB_bindless_texture : enable
#extension GL_ARB_gpu_shader_int64 : enable

//ssbo input
layout(std430, binding = 6) readonly buffer TextureIndices {
    uint textureIndices[];
};

layout(std430, binding = 2) readonly buffer textureHandles {
    uint64_t handles[];
};

// layout(std430, binding = 7) readonly buffer animations {
//     vec2 animation[];
// };

// inputs
layout(location = 0) in vec2 in_texCoord;
layout(location = 1) in flat uint in_instanceID; // = gl_InstanceID + gl_BaseInstance;

out vec4 FragColor;

// Size of the texture atlas (for a 4x4 grid)
const int atlas_grid_size = 9;
// index what texture to get in the atlas
const int atlas_frame = 5;
void main()
{
    // Calculate the row and column of the sub-texture in the atlas
    int row = int(atlas_frame) / atlas_grid_size;
    int col = int(atlas_frame) % atlas_grid_size;

    // get id for texture handle
    uint id = textureIndices[in_instanceID];

    // Adjust the texture coordinates based on the grid position
    vec2 atlasTexCoord = (in_texCoord + vec2(col, row)) / float(atlas_grid_size);

    FragColor = texture(sampler2D(handles[id]), atlasTexCoord);

};