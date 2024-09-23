#ifndef camera_h
#define camera_h

#include "glmath.h"

class camera{
public:
    vec3 position;
    vec3 target;
    vec3 up;
    vec3 offset;

    mat4 view;
    mat4 projection;
    mat4 rotationMatrix;

    int zoom;
    int rotation;
    float radius; // used for rotation;

    camera(uint width, uint height);
    ~camera();
    
    void rotate(float angleAddition);
    void move(vec3 translate);
    void moveTo(vec3 vec);
};

#endif