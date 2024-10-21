#pragma once
#include "glmath.h"

/// @brief a struct with all the information formated be the controller for use with other classes
///
/// @usage: &controller.interface
struct controllerI{
    // vec3 mouseWorldDirection; // world position
    // bool isMouseWorldDirty;

    vec2 mouseScreenPosition; // screen position NDC, // only updates with mouse direction
    bool isMouseScreenDirty;

    bool isJumping;

    bool mouseLeftDown;
    bool mouseRightDown;

    int windowWidth;
    int windowHeight;
};