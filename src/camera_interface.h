#pragma once

#include "glmath.h"

struct cameraI{
    // vec3 position;
    vec3 target;
    // vec3 up;
    // vec3 offset;

    mat4 view;
    mat4 projection;
    // mat4 rotationMatrix;
};