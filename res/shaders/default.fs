// #version 460 core
// #extension GL_ARB_bindless_texture : enable
// #extension GL_ARB_gpu_shader_int64 : enable

// //ssbo input
// layout(std430, binding = 2) readonly buffer textureHandles {
//     uint64_t handles[];
// };

// // inputs
// layout(location = 0) in vec2 in_texCoord;
// layout(location = 1) in flat uint in_textureID;

// out vec4 FragColor;

// // uniform sampler2D u_textureSampler;

// void main()
// {
//     // FragColor = vec4(1,0,0,1);
//     // FragColor = texture(u_textureSampler, in_texCoord);

//     FragColor = texture(sampler2D(handles[0]), in_texCoord);
// };