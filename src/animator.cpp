#include "animator.h"

Animator::Animator(uint count)
{
    animations_play.resize(count);
    animations_shader_play.resize(count);

    // presets
    loadAnimation("null", 0, 1, 1, -1, false);
    loadAnimation("hello", 0, 9, 81, 0.5f, true);
    loadAnimation("hello1", 0, 9, 81, 3.0f, true);

    cout << "creating animator" << endl;
}

Animator::~Animator()
{
    cout << "deleting animator" << endl;
}

void Animator::setAnimation(string name, uint index)
{
    if(animations_play[index].name == name)
    {
        // cout << "already playing that animation" << endl;
        return;
    }

    const auto& um = animations.find(name);

    if (um != animations.end()) 
    {
        // Key found
        animations_play[index] = um->second;
        animations_shader_play[index] = um->second.shaderAnim;
        // cout << "animation change" << endl;
    } 
    else 
    {
        // Key not found
        throw std::runtime_error("Animation not found!");
    }

}

/// @brief 
/// @param name 
/// @param textureIndex 
/// @param atlasGridSize 
/// @param frameCount 
/// @param frameTime 
/// @param loop 
void Animator::loadAnimation(string name, uint textureIndex, uint atlasGridSize, uint frameCount, float frameTime, bool loop)
{
    Animation animation;

    animation.b_loop = loop;
    animation.frame_count = frameCount;
    animation.frame_time = frameTime;
    animation.current_time = 0;

    animation.shaderAnim.atlas_grid_size = atlasGridSize;
    animation.shaderAnim.atlas_frame = 0;
    animation.shaderAnim.texture_index = textureIndex;

    animation.name = name;
    // // size will equal the index
    // animation.animation_shader_index = 0;
    
    animations.insert({name, animation});
}

void Animator::update(float deltaTime)
{   
    int i = 0;
    for(auto& anim : animations_play)
    {   
        if(anim.frame_time > 0.0f)
        {
            anim.current_time += deltaTime;

            if(anim.current_time > anim.frame_time)
            {   
                animations_shader_play[i].atlas_frame++;
                anim.current_time = 0;
            }

            if(anim.b_loop && anim.frame_count < animations_shader_play[i].atlas_frame)
            {
                animations_shader_play[i].atlas_frame = 0;
            }
        }
        i++;
        
    }
}

void Animator::play(uint index, string name)
{
    auto um = animations.find(name);
    // animations_play.insert({name, um->second});
}

void Animator::stop(uint index, string name)
{
    // animations_play.erase(name);
}

unsigned int Animator::getBufferSize()
{
    return animations_shader_play.size() * sizeof(Anim);
}

const void *Animator::getBufferData()
{
    return animations_shader_play.data();
}