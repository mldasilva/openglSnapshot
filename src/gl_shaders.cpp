#include "gl_shaders.h"

// =======================================================================================
//
// Shader
//
// =======================================================================================

std::string LoadFileToString(const std::string& filePath) {
    // Open the file in binary mode (optional, but ensures no newline translation)
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    
    // Check if the file is open and ready to read
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filePath << std::endl;
        return "";
    }

    // Create a string stream and read the file into it
    std::stringstream buffer;
    buffer << file.rdbuf(); // Read the entire file buffer into the stringstream
    
    // Close the file
    file.close();

    // Return the contents as a string
    return buffer.str();
}

shader::shader(const char* vertexPath, const char* fragmentPath)
{
    cout << "creating shader object..." << endl;

    ssboIndex = 0; // keeping track of ssbo vector pushs

    // 1. retrieve the vertex/fragment source code from filePath
    string vertexCode;
    string fragmentCode;

    // ifstream vShaderFile;
    // ifstream fShaderFile;

    vertexCode      = LoadFileToString(vertexPath);
    fragmentCode    = LoadFileToString(fragmentPath);
    // ensure ifstream objects can throw exceptions:
    // vShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    // fShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    // try 
    // {
    //     std::string vp, fp;
    //     vp = vertexPath;
    //     fp = fragmentPath;

    //     // open files
    //     vShaderFile.open(vp);
    //     fShaderFile.open(fp);

    //     stringstream vShaderStream, fShaderStream;

    //     // read file's buffer contents into streams
    //     vShaderStream << vShaderFile.rdbuf();
    //     fShaderStream << fShaderFile.rdbuf();

    //     // close file handlers
    //     vShaderFile.close();
    //     fShaderFile.close();

    //     // convert stream into string
    //     vertexCode   = vShaderStream.str();
    //     fragmentCode = fShaderStream.str();
    // }
    // catch (ifstream::failure& e)
    // {
    //     cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << endl;
    // }

    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // shader Program
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    checkCompileErrors(id, "PROGRAM");

    // two gl uniforms that will be const in camera shaders
    set_uniform_location("u_view");
    set_uniform_location("u_projection");

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

shader::~shader()
{
    cout << "deleting shader object" << endl;

        // vector<uint> ssbo;
    // vector<void*> ssbo_map;
    for(uint s : ssbo)
    {
        cout << "deleting ssbo "<< s << endl;
        glDeleteBuffers(1, &s);
    }

    // ssbo_map not required? doesnt use a glGenFunction
}

void checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

// void update_position(uint index, shader& shader, renderPool& render, vec3 position)
// {
//     render.matrices[index] = translate(mat4(1.0f), position);
//     shader.update_ssbo(index, sizeof(mat4), &render.matrices[index]);
// }

void shader::draw(camera& camera, bufferObject& buffer)
{
    // Use the shader program
    glUseProgram(id);

    // move into get_uniform_location
    // Get the location of the 'modelMatrix' uniform in the shader
    // GLint viewmatrix = glGetUniformLocation(id, "u_view");
    // GLint projectionmatrix = glGetUniformLocation(id, "u_projection");

    // texture slot example
    // glUniform1i(glGetUniformLocation(id, "u_textureSampler"), 3);

    // Pass the glm::mat4 to the shader
    glUniformMatrix4fv(get_uniform_location("u_view"), 1, GL_FALSE, glm::value_ptr(camera.view));
    glUniformMatrix4fv(get_uniform_location("u_projection"), 1, GL_FALSE, glm::value_ptr(camera.projection));
    
    glBindVertexArray(buffer.VAO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer.indirectBuffer);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, buffer.commandCount, sizeof(DrawElementsIndirectCommand));

}

//CREATE_SHADER_BUFFER_STORAGE
void shader::create_ssbo(uint binding, uint size, const void * data)
{
    if (data == NULL) 
    {
        // cout << "!!ssbo data is empty!!!" << endl;
        throw std::runtime_error("!!ssbo data is empty!!!");
        return;
    }
    // cout << "creating ssbo..." << endl;
    // ssbo.push_back(0);
    // ssbo_map.push_back(0);

    // glGenBuffers(1, &ssbo[ssboIndex]); // by address
    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[ssboIndex]);
    // glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, data, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo[ssboIndex]); // by value
    // // ssbo_map[ssboIndex] = 
    // ssbo_map.push_back(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
    // glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // ssboIndex++;

    GLuint buffer;                 // New SSBO handle
    glGenBuffers(1, &buffer);      // Generate the SSBO
    ssbo.push_back(buffer);        // Add the handle to the vector

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, data, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);

    // Map the buffer and store the mapped pointer in the vector
    void* mappedBuffer = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    ssbo_map.push_back(mappedBuffer); // Store the mapped pointer

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind the buffer

    ssboIndex++;
}

void shader::update_ssbo(uint index, uint size, const void *data)
{
    memcpy(ssbo_map[index], data, size);
}

void shader::triangle_debug()
{
    glUseProgram(0);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(0.5f, -0.5f);
    glEnd();
}

void shader::set_uniform_location(const char *name)
{
    int location = glGetUniformLocation(id, name);
    uniformLocationMap[name] = location;
}

int shader::get_uniform_location(const char *name)
{
    if(uniformLocationMap.find(name) != uniformLocationMap.end())
    {
        return uniformLocationMap[name];
    }
    else
    {
        cout << "uniform location not set" << endl;
        return -1;
    }
}

// =======================================================================================
//
// Texture
//
// =======================================================================================

texture::texture()
{
    cout << "creating texture object..." << endl;
    textureAddrIndex = 0;
    bindless_index = 0;
}

texture::~texture()
{
    for(const uint& id : textureAddress) 
    {
        cout << "deleting texture " << id << endl;
        glDeleteTextures(1, &id);
    }

    cout << "deleting texture object..." << endl;
}

/// @brief loads texture into a opengl texture slot, default 0
/// @param _textureAddr_ Specifies an address (array) in which the generated texture names are stored.
/// @param _textureSlot_ Specifies which texture unit to make active. The number of texture units is implementation dependent, but must be at least 80
/// @param _filepath____ stbi_load filepath of the texture
void texture::loadTexture(const char * filepath, uint textureSlot)
{
    textureAddress.push_back(0);
    cout << "loaded texture into address: " << textureAddrIndex << endl;
    //load and create a texture 
    //-------------------------
    glGenTextures(1, &textureAddress[textureAddrIndex]);
    // The initial value is GL_TEXTURE0.
    glActiveTexture(GL_TEXTURE0 + textureSlot); // controlling texture slot to bind into
    glBindTexture(GL_TEXTURE_2D, textureAddress[textureAddrIndex]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    // settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // or GL_Linear vs GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // or GL_Linear vs GL_NEAREST

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // or GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // or GL_CLAMP_TO_EDGE
    
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    
    if (data)
    {
        //GL_RGBA8 and GL_RGBA are for pngs
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        //glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load image");
    }

    stbi_image_free(data);

    textureAddrIndex++;
}

/// @brief overload/ override for vram bindless textures
/// @param textures 
/// @param textureHandle 
/// @param filepath 
void texture::loadTexture(const char * filepath)
{
    //load and create a texture 
    //-------------------------
    textureAddress.push_back(0);
    bindless_texture_handles.push_back(0);

    glGenTextures(1, &textureAddress[textureAddrIndex]);
    glBindTexture(GL_TEXTURE_2D, textureAddress[textureAddrIndex]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    // settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // or GL_Linear vs GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // or GL_Linear vs GL_NEAREST

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // or GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // or GL_CLAMP_TO_EDGE
    
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // stbi_set_flip_vertically_on_load(1); // i think gltf importer is flipping uvs
    unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    
    if (data)
    {
        //GL_RGBA8 and GL_RGBA are for pngs
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        //glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load image");
    }

    stbi_image_free(data);

    bindless_texture_handles[bindless_index] = glGetTextureHandleARB(textureAddress[textureAddrIndex]);
    glMakeTextureHandleResidentARB(bindless_texture_handles[bindless_index]);

    textureAddrIndex++;
    bindless_index++;
}

GLuint64 *texture::handles()
{
    return bindless_texture_handles.data();
}

uint texture::handleByteSize()
{
    return bindless_texture_handles.size() * sizeof(GLuint64);
}

