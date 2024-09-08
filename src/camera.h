#ifndef camera_h
#define camera_h


#include "glmath.h"

class camera{
public:
    vec3 position;
    vec3 target;
    vec3 up;

    mat4 view;
    mat4 projection;

    int zoom;
    int rotation;
    float radius; // used for rotation;

    camera(uint width, uint height);
    void rotate(float angleAddition);
};

#endif