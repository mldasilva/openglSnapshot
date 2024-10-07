#version 460 core
#extension GL_ARB_bindless_texture : enable
#extension GL_ARB_gpu_shader_int64 : enable

struct Anim{
    int atlas_grid_size;
    int texture_index;
    int atlas_frame; // current frame
};

layout(std430, binding = 2) readonly buffer ssbo_textureHandles {
    uint64_t handles[];
};

layout(std430, binding = 7) readonly buffer ssbo_animations {
    Anim animation[];
};

// inputs
layout(location = 0) in vec2 in_texCoord;
layout(location = 1) in flat uint in_instanceID; // = gl_InstanceID + gl_BaseInstance;

out vec4 FragColor;

void main()
{   
    int atlas_grid_size = animation[in_instanceID].atlas_grid_size;
    // Calculate the row and column of the sub-texture in the atlas
    int row = int(animation[in_instanceID].atlas_frame) / atlas_grid_size;
    int col = int(animation[in_instanceID].atlas_frame) % atlas_grid_size;

    // get id for texture handle
    uint id = animation[in_instanceID].texture_index;

    // Adjust the texture coordinates based on the grid position
    vec2 atlasTexCoord = (in_texCoord + vec2(col, row)) / float(atlas_grid_size);

    FragColor = texture(sampler2D(handles[id]), atlasTexCoord);

};