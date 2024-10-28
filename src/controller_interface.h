#pragma once
#include "glmath.h"

/// @brief a struct with all the information formated be the controller for use with other classes
///
/// @usage: &controller.interface
struct controllerI{
    // vec3 mouseWorldDirection; // world position
    // bool isMouseWorldDirty;

    vec2 mouseScreenPosition; // screen position NDC, // only updates with mouse left click
    bool isMouseScreenDirty = false;

    bool isJumping = false;

    bool mouseLeftDown = false;
    bool mouseRightDown = false;

    int windowWidth;
    int windowHeight;

    bool isMouseInUI = false; // changed in user_interface.cpp
    vec2 rawMouseScreenPos; // always updated
};