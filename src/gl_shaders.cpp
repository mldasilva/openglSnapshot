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

DaSilva::Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // cout << "creating shader object..." << endl;

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
    //     cout << "ERROR::Managers::Managers::Managers::Shader::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << endl;
    // }

    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
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

DaSilva::Shader::~Shader()
{
    // cout << "deleting shader object" << endl;

    // vector<uint> ssbo;
    // vector<void*> ssbo_map;
    for(uint &s : ssbo)
    {   
        // bind the buffer for unmapping
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, s); 
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        // cout << "deleting ssbo "<< s << endl;
        glDeleteBuffers(1, &s);
    }

    // set pointers to nullptr to avoid dangling pointers
    for(auto &ptr : ssbo_map)
    {
        ptr = nullptr;
    }
    
    
    // ssbo_map not required? doesnt use a glGenFunction
}

void DaSilva::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

// void update_position(uint index, shader& shader, renderPool& render, vec3 position)
// {
//     render.matrices[index] = translate(mat4(1.0f), position);
//     shader.update_ssbo(index, sizeof(mat4), &render.matrices[index]);
// }

void DaSilva::Shader::draw(Camera& camera, BufferObject& buffer)
{
    // Use the shader program
    glUseProgram(id);

    // move into get_uniform_location
    // Get the location of the 'modelMatrix' uniform in the shader
    // GLint viewmatrix = glGetUniformLocation(id, "u_view");
    // GLint projectionmatrix = glGetUniformLocation(id, "u_projection");

    // texture slot example
    // glUniform1i(glGetUniformLocation(id, "u_textureSampler"), 3);
    
    // glm::mat4 trans = glm::mat4(1.0f);
    // trans = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    // trans = glm::scale(trans, glm::vec3(0.1f, 0.1777f, 1.0f)); 
    // trans = glm::translate(trans, glm::vec3(0.0f, 3.0f, 0.0f));

    // unsigned int transformLoc = glGetUniformLocation(id, "transform");
    // glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    // Pass the glm::mat4 to the shader
    glUniformMatrix4fv(get_uniform_location("u_view"), 1, GL_FALSE, glm::value_ptr(camera.view));
    glUniformMatrix4fv(get_uniform_location("u_projection"), 1, GL_FALSE, glm::value_ptr(camera.projection));
    
    // glUniform1i(get_uniform_location("u_textureSampler"), 0); // test for sampler2d non bindless texture alternative

    glBindVertexArray(buffer.VAO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer.indirectBuffer);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, buffer.commandCount, sizeof(DrawElementsIndirectCommand));

}

void DaSilva::Shader::use()
{
    glUseProgram(id);
}

//CREATE_SHADER_BUFFER_STORAGE
void DaSilva::Shader::create_ssbo(uint binding, uint size, const void* data)
{
    if (data == nullptr) 
    {
        // cout << "!!ssbo data is empty!!!" << endl;
        throw std::runtime_error("!!ssbo data is empty!!!");
        return;
    }
    ssbo_size.push_back(size); // used for updating
    ssbo_data.push_back(data); // used for updating
/*
    GL_MAP_PERSISTENT_BIT: This flag allows you to keep the buffer persistently mapped, 
    meaning that the pointer returned by glMapBufferRange remains valid until you explicitly unmap it. 
    You do not need to call glUnmapBuffer after mapping it with this flag unless you no longer need the mapping 
    and want to unmap it permanently.

    GL_MAP_COHERENT_BIT: Ensures that any writes from the CPU side to the mapped buffer are automatically 
    visible to the GPU without needing explicit synchronization commands (like memory barriers).
*/
    GLuint buffer;                 // New SSBO handle
    glGenBuffers(1, &buffer);      // Generate the SSBO
    ssbo.push_back(buffer);        // Add the handle to the vector

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, data, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer);

    // Map the buffer and store the mapped pointer in the vector
    void* mappedBuffer = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    ssbo_map.push_back(mappedBuffer); // Store the mapped pointer

    // glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind the buffer
    
    ssboIndex++;
}

void DaSilva::Shader::update_ssbo(uint index)
{
    memcpy(ssbo_map[index], ssbo_data[index], ssbo_size[index]);
}

void DaSilva::Shader::triangle_debug()
{
    glUseProgram(0);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(0.5f, -0.5f);
    glEnd();
}

void DaSilva::Shader::set_uniform_location(const char *name)
{
    int location = glGetUniformLocation(id, name);
    uniformLocationMap[name] = location;
}

int DaSilva::Shader::get_uniform_location(const char *name)
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

uint DaSilva::Shader::getID()
{
    return id;
}

// =======================================================================================
//
// Texture
//
// =======================================================================================

DaSilva::Texture::Texture()
{
    // cout << "creating texture object..." << endl;
    textureAddrIndex = 0;
    bindless_index = 0;
}

DaSilva::Texture::~Texture()
{
    for(const uint& id : textureAddress) 
    {
        // cout << "deleting texture " << id << endl;
        glDeleteTextures(1, &id);
    }

    // cout << "deleting texture object..." << endl;
}

/// @brief loads texture into a opengl texture slot, default 0
/// @param _textureAddr_ Specifies an address (array) in which the generated texture names are stored.
/// @param _textureSlot_ Specifies which texture unit to make active. The number of texture units is implementation dependent, but must be at least 80
/// @param _filepath____ stbi_load filepath of the texture
void DaSilva::Texture::loadTexture(const char * filepath, uint textureSlot)
{
    textureAddress.push_back(0);
    // cout << "loaded texture into address: " << textureAddrIndex << endl;
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
void DaSilva::Texture::loadTexture(const char * filepath)
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

/// @brief can call only once per instance of object.
/// 
/// disadvantage vs bindless textures is Texture-Array require all textures to be the same size else result in seg fault
/// @param filepaths 
void DaSilva::Texture::loadTextureArray(string* filepaths)
{
    // GLuint textureArray;
    glGenTextures(1, &textureArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

    // Define the array size: width, height, and number of layers (textures)
    int width = 1024; // Width of each texture
    int height = 1024; // Height of each texture
    int layers = filepaths->size();  // Number of textures
    
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, layers);  // Allocate storage

    int temp_width, temp_height, temp_nrChannels;
    
    // Load each texture layer (assuming you have already loaded the texture data)
    for (int i = 0; i < layers; ++i) {
        // Assuming you have texture data in an array of textures:
        stbi_set_flip_vertically_on_load(1);
        unsigned char *data = stbi_load(filepaths[i].c_str(), &temp_width, &temp_height, &temp_nrChannels, 0);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    // Set texture parameters (min/mag filtering, wrapping, etc.)
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void DaSilva::Texture::bindTextureArray(uint slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
}

GLuint64 *DaSilva::Texture::getBufferData()
{
    return bindless_texture_handles.data();
}

uint DaSilva::Texture::getBufferSize()
{
    return bindless_texture_handles.size() * sizeof(GLuint64);
}

