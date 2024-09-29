#pragma once

#include "glmath.h"
#include "typedef.h"
#include <iostream>
#include <unordered_map>

using namespace std;

struct Anim{
    int atlas_grid_size;
    int texture_index;
    int atlas_frame; // current frame
};

struct Animation{
    uint frame_count; // how many frames
    float frame_time; // how long to stay on a frame, before switching to the next frame
    bool b_loop;
    float current_time;

    uint animation_shader_index = 0; // Anim vector index;
};

class Animator{
    private:
        vector<Anim> animations_shader;
        unordered_map<string, Animation> animations;
        float time;
        
    public:
        Animator();
        ~Animator();

        void addAnimation(string name, uint textureIndex, uint atlasGridSize, uint frameCount, float frameTime, bool loop);
        void update(vec3 innputDirection, float deltaTime);
        void play();
        void stop();

        unsigned int getBufferSize();
        const void* getBufferData();

        uint find(string name);
};