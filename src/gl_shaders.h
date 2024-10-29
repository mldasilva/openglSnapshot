#ifndef gl_shaders_h
#define gl_shaders_h

#define GLEW_STATIC
#include <glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <vector>
#include <string>
#include <unordered_map>

#include "glmath.h"
#include "camera.h"
#include "render.h"

#include <stb_image.h>

namespace DaSilva{

    void checkCompileErrors(unsigned int shader, std::string type);
    
    class Shader{
        private:
            unordered_map<string, int> uniformLocationMap;
            uint ssboIndex; // keeping track of ssbo and sbbomap pushs

            vector<const void*> ssbo_data; // pointers to data
            vector<uint> ssbo_size;

            uint id;
            vector<uint> ssbo;
            vector<void*> ssbo_map;

        public:
            Shader(const char* vertexPath, const char* fragmentPath);
            ~Shader();

            void create_ssbo(uint binding, uint size, const void * data);
            void update_ssbo(uint index);
            void draw(Camera& camera, BufferObject& buffer);
            void use();
            void triangle_debug();
            void set_uniform_location(const char *name);
            int  get_uniform_location(const char *name);
            uint getID();


    };

    // Singleton
    // usage: Textures& textures = Textures::getInstance();
    class Textures{
        private:
            vector<uint> textureAddress;
            uint textureAddrIndex;  // keeping track of which textureAddress to push into
            
            uint bindless_index;    // keeping track of which bindless_texture_handles to push into

            vector<GLuint64> bindless_texture_handles;

            uint textureArrayCount = 0;
            vector<uint> texture_arrays;

            // Private constructor to prevent instantiation
            Textures();
            ~Textures();
        public:
            // Public method to access the single instance
            static Textures& getInstance() {
                static Textures instance;  // Guaranteed to be destroyed, instantiated on first use
                return instance;
            }

            // Delete the copy constructor and assignment operator
            // When you use = delete;, 
            // it informs the compiler and other programmers that copying instances of the Textures class is not allowed
            Textures(const Textures&) = delete;
            Textures& operator=(const Textures&) = delete;

            void loadTexture(const char * filepath, uint textureSlot);
            void loadTexture(const char * filepath);
            void loadTextureArray(vector<string> filepaths);
            void bindTextureArray(uint slot, uint index);

            GLuint64* getBufferData();
            uint getBufferSize();
    };


} // namespace






// void update_position(uint index, shader& shader, renderPool& render, vec3 position);
#endif