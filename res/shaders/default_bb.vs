#version 460 core

// //ssbo
layout(std430, binding = 4) readonly buffer InstanceMatricies {
    vec3 instancedMatrix;
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

vec2 billboardSize = vec2(1.25,1.25);
vec3 billboardPos = vec3(0,3,0);

void main()
{
    // https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/
    // In camera space, the camera’s up vector is (0,1,0). To get it in world space, just multiply this by the matrix that goes 
    // from Camera Space to World Space, which is, of course, the inverse of the View matrix.
    // An easier way to express the same math is :
    vec3 u_cameraRight_worldspace   = vec3(u_view[0][0], u_view[1][0], u_view[2][0]);
    vec3 u_cameraUp_worldspace      = vec3(u_view[0][1], u_view[1][1], u_view[2][1]);

    // this becomes the position
    vec3 vertexPosition_worldspace = instancedMatrix 
    + u_cameraRight_worldspace * Position.x * billboardSize.x 
    + u_cameraUp_worldspace * Position.y * billboardSize.y;

    mat4 mvp        = u_projection * u_view; // dont need model 
    gl_Position     = mvp * vec4(vertexPosition_worldspace, 1);

    out_texCoord    = TexCoord;
    out_textureID   = gl_InstanceID + gl_BaseInstance;
};