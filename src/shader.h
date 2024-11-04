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

// Singleton
// usage: ShaderStorageBufferObject& ssbo = ShaderStorageBufferObject::getInstance();
class ShaderStorageBufferObject
{
    private:
        unordered_map<string, int> ssboIDS; // ssnoName to IDS

        uint ssboCount = 0; // used as ssbo id

        vector<uint>        ssbo;       // handles
        vector<void*>       ssbo_map;
        vector<uint>        ssbo_size;
        vector<const void*> ssbo_data;  // pointers to data
        
        void create_ssbo(uint binding, uint size, const void* data);
        void update_ssbo(uint index);
        
        // Private constructor to prevent instantiation
        ShaderStorageBufferObject();
        ~ShaderStorageBufferObject();

    public:
        
        // Public method to access the single instance
        static ShaderStorageBufferObject& getInstance() 
        {
            // Guaranteed to be destroyed, instantiated on first use
            static ShaderStorageBufferObject instance;  
            return instance;
        }
        // Delete the copy constructor and assignment operator
        // When you use = delete;, 
        // it informs the compiler and other programmers that copying instances of the Textures class is not allowed
        ShaderStorageBufferObject(const ShaderStorageBufferObject&) = delete;
        ShaderStorageBufferObject& operator=(const ShaderStorageBufferObject&) = delete;

        bool add(string ssboName, uint size, const void* data);
        bool add(string ssboName, storageStruct input);
        int find(string ssboName);

        // use with caution
        void updateAll();

        //proper update
        bool update(string name);
};

// shader v2 uses ShaderStorageBufferObject class for global ssbo management
class Shader_2{
    private:
        uint id;
        string vertexCode;
        string fragmentCode;

        unordered_map<string, int> uniformLocationMap;

        void triangle_debug();
        void checkCompileErrors(unsigned int shader, std::string type);

        void set_uniform_location(const char *name);
        int  get_uniform_location(const char *name);

        string debug_vertShaderOut();
        string debug_fragShaderOut();

        void use();
        uint getID();

    public:
        Shader_2(const char* vertexPath, const char* fragmentPath);
        ~Shader_2();

        void draw(Camera& camera, DaSilva::BufferObject& buffer, int textureSlot);
        void draw(DaSilva::BufferObject& buffer, int textureSlot);

        void attachSSBO(int binding, shaderTypeEnum shaderType);
        void attachSSBO(int vsBinding, int fsBinding);
        void linkProgram(bool isCamera);
};