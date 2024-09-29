#include "animator.h"

Animator::Animator()
{
    cout << "creating animator" << endl;
}

Animator::~Animator()
{
    cout << "deleting animator" << endl;
}

/// @brief 
/// @param name 
/// @param textureIndex 
/// @param atlasGridSize 
/// @param frameCount 
/// @param frameTime 
/// @param loop 
void Animator::addAnimation(string name, uint textureIndex, uint atlasGridSize, uint frameCount, float frameTime, bool loop)
{
    Animation animation;
    Anim animationShader;

    animation.b_loop = loop;
    animation.frame_count = frameCount;
    animation.frame_time = frameTime;
    animation.current_time = 0;

    animationShader.atlas_grid_size = atlasGridSize;
    animationShader.atlas_frame = 0;
    animationShader.texture_index = textureIndex;

    // size will equal the index
    animation.animation_shader_index = animations_shader.size();
    
    animations.insert({name, animation});
    animations_shader.push_back(animationShader);
}

void Animator::update(vec3 innputDirection, float deltaTime)
{
    for(auto& anim : animations)
    {
        animations[anim.first].current_time += deltaTime;

        if(animations[anim.first].current_time > animations[anim.first].frame_time)
        {
            
            animations_shader[animations[anim.first].animation_shader_index].atlas_frame++;
            animations[anim.first].current_time = 0;

            // cout << "frame change." << animations_shader[animations[anim.first].animation_shader_index].atlas_frame<< endl;
        }
        
    }
}

unsigned int Animator::getBufferSize()
{
    return animations_shader.size() * sizeof(Anim);
}

const void *Animator::getBufferData()
{
    return animations_shader.data();
}

uint Animator::find(string name)
{
    auto um = animations.find(name);
    // cout << "Iterator points to " << it->first << " = " << it->second << endl;
    return um->second.animation_shader_index;
}
