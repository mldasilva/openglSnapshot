#version 460 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D u_textureSampler;

void main()
{
    gl_FragDepth = 0; // 0 front 1 back
    FragColor = texture(u_textureSampler, TexCoord);
}