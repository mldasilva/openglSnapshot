#pragma once

#include "typedef.h"
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

struct Anim{
    int atlas_grid_size;
    int texture_index;
    int atlas_frame; // current frame // requires updating
};

struct Animation{
    uint frame_count; // how many frames
    float frame_time; // how long to stay on a frame, before switching to the next frame
    bool b_loop;
    float current_time;

    Anim shaderAnim;    // shader package

    // bool playing;
    string name;
};


class Animator{
    private:
        unordered_map<string, Animation> animations;    // to hold all

        vector<Animation> animations_play;              // hold playing animations
        vector<Anim> animations_shader_play;            // info for shader

        void loadAnimation(string name, uint textureIndex, uint atlasGridSize, uint frameCount, float frameTime, bool loop);

    public:
        Animator(uint count);
        ~Animator();

        unsigned int getBufferSize();
        const void* getBufferData();

        void update(float deltaTime);

        void setAnimation(string name, uint index);

        void play(uint index, string name);
        void stop(uint index, string name);

        // void* interface;
};


