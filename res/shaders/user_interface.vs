#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

layout(std430, binding = 10) readonly buffer InstanceMatricies {
    mat4 instancedMatrix[];
};

out vec3 ourColor;
out vec2 TexCoord;
out flat int layerIndex;

uniform mat4 transform;

void main()
{
    gl_Position = instancedMatrix[gl_BaseInstance + gl_InstanceID] * vec4(aPos, 1.0f);
    ourColor = aColor;
    TexCoord = aTexCoord;
    layerIndex = gl_BaseInstance + gl_InstanceID;
}