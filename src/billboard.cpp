#include "billboard.h"

Billboard::Billboard(float halfSize)
{
	// float vertexData[] = {
    //     // positions          // colors           // texture coords
    //     o,  o, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
    //     o, -o, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    //     -o, -o, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    //     -o,  o, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    // };

    // swapping order of indices draws the plane with the normal facing the camera 
    // when the billboard shader is applied, so i no longer need to disable depth culling
    // unsigned int indices[] = {  // note that we start from 0!
    //     3,1,0,   // first triangle
    //     3,2,1    // second triangle
    // };

	vertex ver;

    indices.push_back(3);
    indices.push_back(1);
    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(2);
    indices.push_back(1);

    ver.position = vec3(halfSize, halfSize, 0);
    ver.normal = vec3(0,0,0);
    ver.texUV = vec2(1.0, 1.0);

    vertices.push_back(ver);

    ver.position = vec3(halfSize, -halfSize, 0);
    ver.normal = vec3(0,0,0);
    ver.texUV = vec2(1.0, 0.0);

    vertices.push_back(ver);

    ver.position = vec3(-halfSize, -halfSize, 0);
    ver.normal = vec3(0,0,0);
    ver.texUV = vec2(0.0, 0.0);

    vertices.push_back(ver);

    ver.position = vec3(-halfSize, halfSize, 0);
    ver.normal = vec3(0,0,0);
    ver.texUV = vec2(0.0, 1.0);

    vertices.push_back(ver);
}

// modelI Billboard::getInterface()
// {
// 	modelI data;

// 	data.vertices = vertices;
// 	data.indices = indices;

//     return data;
// }