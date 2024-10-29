#pragma once

#define GLEW_STATIC
#include <glew.h>

#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glmath.h"
#include "camera.h"
#include "render.h"

enum shaderTypeEnum
{
    none, vertShader, fragShader
};
//==========================================
//=        Version 2 of gl_shaders
//==========================================
// difference is this shader manager will manage shader code and shader buffer objects binds 
// allowing multiple shaders of the same code to use different ssbos

class ShaderStorageBufferObject
{
    private:
        unordered_map<string, int> ssboIDS; // ssnoName to IDS

        uint ssboCount = 0; // not in use might remove

        vector<uint> ssbo; // handles
        vector<void*> ssbo_map;
        vector<uint> ssbo_size;
        vector<const void*> ssbo_data; // pointers to data
        
        void create_ssbo(uint binding, uint size, const void* data);
        void update_ssbo(uint index);

    public:
        ShaderStorageBufferObject();
        ~ShaderStorageBufferObject();
        
        bool add(string ssboName, uint size, const void* data);
        int find(string ssboName);

        // use with caution
        void updateAll();
};

// shader v2 uses ShaderStorageBufferObject class for global ssbo management
class Shader_2{
    private:
        uint id;

        uint vertex, fragment;
        string vertexCode;
        string fragmentCode;

        unordered_map<string, int> uniformLocationMap;

        void checkCompileErrors(unsigned int shader, std::string type);
        void replaceBindings(int ssboCount, shaderTypeEnum shaderType);
        
    public:
        Shader_2(const char* vertexPath, const char* fragmentPath);
        ~Shader_2();

        void draw(Camera& camera, DaSilva::BufferObject& buffer, int textureSlot);
        void use();
        void triangle_debug();
        void set_uniform_location(const char *name);
        int  get_uniform_location(const char *name);
        uint getID();

        string debugVertShaderOut();
        string debugFragShaderOut();

        bool attachSSBO(int binding, shaderTypeEnum shaderType);
        bool attachCode(shaderTypeEnum shaderType);
        bool linkProgram();
};