#version 460 core
out vec4 FragColor;

struct textureParamUI
{
    vec4 subRegionStart; // vec2
    vec4 subRegionSize;  // vec2
    vec4 textureSize;    // vec2
    vec4 textureIndex;   // int
};

layout(std430, binding = 11) readonly buffer LayerIndices {
    textureParamUI textureParams[];
};

in vec3 ourColor;
in vec2 TexCoord;  // Original texture coordinates (0.0 to 1.0)
in flat int layerIndex;

uniform sampler2DArray u_textureArray;  // The texture array contains all the textures

void main()
{
    // Optional: Set depth if required (else remove this)
    gl_FragDepth = 0;

    // Get the texture parameters for the current layer
    textureParamUI tpu = textureParams[layerIndex];
    
    // glm padding to opengl size is messed up couldnt figure it out had to brute force it abit
    vec2 subRegionStart = vec2(tpu.subRegionStart.x, tpu.subRegionStart.y);
    vec2 subRegionSize  = vec2(tpu.subRegionSize.x, tpu.subRegionSize.y);
    vec2 textureSize    = vec2(tpu.textureSize.x, tpu.textureSize.y);

    // Calculate the new texture coordinates targeting the sub-region
    vec2 newTexCoord = subRegionStart / textureSize 
                     + TexCoord * (subRegionSize / textureSize);
    
    // Sample the texture array using the newTexCoord, and the textureIndex for the layer
    FragColor = texture(u_textureArray, vec3(newTexCoord, tpu.textureIndex.x));
}
