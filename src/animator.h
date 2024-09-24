#pragma once

#include "glmath.h"
#include "typedef.h"
#include <iostream>

using namespace std;

struct Animation{
    uint frame_count; // how many frames
    float frame_time; // how long to stay on a frame, before switching to the next frame
    bool b_loop;

    vec2 atlas_grid_size;
    uint atlas_frame;
};

class Animator{
    private:
        vector<Animation> animations;
    public:
        Animator();
        ~Animator();

        
        void update(vec3 innputDirection, float deltaTime);
};