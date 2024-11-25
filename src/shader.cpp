#include "shader.h"

// =======================================================================================
//
// Shader Storage Buffer Object
// keep as singlton
// =======================================================================================

ShaderStorageBufferObject::ShaderStorageBufferObject()
{

}

ShaderStorageBufferObject::~ShaderStorageBufferObject()
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

/// @brief 
/// @param ssboName 
/// @param size 
/// @param data 
/// @return true is successfully added
bool ShaderStorageBufferObject::add(string ssboName, uint size, const void *data)
{
    const auto _ssbo = ssboIDS.find(ssboName);
            
    if (_ssbo == ssboIDS.end()) 
    {
        // std::cout << "ssbo not found" << std::endl; 
        ssboIDS.insert({ssboName, ssboCount});
        create_ssbo(ssboCount, size, data);
        ssboCount++;

        return true;
    }
     
    return false;
}

bool ShaderStorageBufferObject::add(string ssboName, storageStruct input)
{
    const auto _ssbo = ssboIDS.find(ssboName);
            
    if (_ssbo == ssboIDS.end()) 
    {
        // std::cout << "ssbo not found" << std::endl; 
        ssboIDS.insert({ssboName, ssboCount});
        create_ssbo(ssboCount, input.size, input.data);
        ssboCount++;

        return true;
    }
     
    return false;
}

/// @brief 
/// @param ssboName 
/// @return ssbo binding index
int ShaderStorageBufferObject::find(string ssboName)
{
    if (ssboIDS.find(ssboName) != ssboIDS.end()) 
    {
        // Key found
        return ssboIDS[ssboName];
    } 

    return -1;
}

void ShaderStorageBufferObject::updateAll()
{
    for (size_t i = 0; i < ssboCount; i++)
    {
        update_ssbo(i);
    }
}

/// @brief 
/// @param name ssbo name
/// @return true is successful
bool ShaderStorageBufferObject::update(string name)
{
    auto entry = ssboIDS.find(name);
    if (entry != ssboIDS.end()) 
    {
        memcpy(ssbo_map[entry->second], ssbo_data[entry->second], ssbo_size[entry->second]);
        return true;
    }
    return false;
}

//CREATE_SHADER_BUFFER_STORAGE
void ShaderStorageBufferObject::create_ssbo(uint binding, uint size, const void* data)
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
}

void ShaderStorageBufferObject::update_ssbo(uint index)
{
    memcpy(ssbo_map[index], ssbo_data[index], ssbo_size[index]);
}

// =======================================================================================
//
// Shader
//
// =======================================================================================

std::string LoadFileToStringv2(const std::string& filePath) {
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

Shader_2::Shader_2(const char* vertexPath, const char* fragmentPath)
{
    // cout << "creating shader object..." << endl;

    // 1. retrieve the vertex/fragment source code from filePath
    vertexCode      = LoadFileToStringv2(vertexPath);
    fragmentCode    = LoadFileToStringv2(fragmentPath);


    // const char* vShaderCode = vertexCode.c_str();
    // const char * fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    // unsigned int vertex, fragment;

    // vertex shader
    // vertex = glCreateShader(GL_VERTEX_SHADER);
    // glShaderSource(vertex, 1, &vShaderCode, nullptr);
    // glCompileShader(vertex);
    // checkCompileErrors(vertex, "VERTEX");

    // fragment Shader
    // fragment = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragment, 1, &fShaderCode, nullptr);
    // glCompileShader(fragment);
    // checkCompileErrors(fragment, "FRAGMENT");

    // shader Program
    // id = glCreateProgram();
    // glAttachShader(id, vertex);
    // glAttachShader(id, fragment);
    // glLinkProgram(id);
    // checkCompileErrors(id, "PROGRAM");

    // two gl uniforms that will be const in camera shaders
    // set_uniform_location("u_view");
    // set_uniform_location("u_projection");

    // delete the shaders as they're linked into our program now and no longer necessary
    // glDeleteShader(vertex);
    // glDeleteShader(fragment);
}

Shader_2::~Shader_2()
{

}

void Shader_2::checkCompileErrors(unsigned int shader, std::string type)
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

void Shader_2::draw(Camera& camera, DaSilva::BufferObject& buffer, int textureSlot)
{
    // Use the shader program
    glUseProgram(id);

    // Pass the glm::mat4 to the shader
    glUniformMatrix4fv(get_uniform_location("u_view"), 1, GL_FALSE, glm::value_ptr(camera.view));
    glUniformMatrix4fv(get_uniform_location("u_projection"), 1, GL_FALSE, glm::value_ptr(camera.projection));
    
    glUniform1i(get_uniform_location("u_textureArray"), textureSlot);
    // glUniform1i(get_uniform_location("u_textureSampler"), 0); // test for sampler2d non bindless texture alternative

    buffer.draw();
}

void Shader_2::draw(DaSilva::BufferObject& buffer, int textureSlot)
{
    // Use the shader program
    glUseProgram(id);

    glUniform1i(get_uniform_location("u_textureArray"), textureSlot);
    // glUniform1i(get_uniform_location("u_textureSampler"), 0); // test for sampler2d non bindless texture alternative

    buffer.draw();
}

void Shader_2::use()
{
    glUseProgram(id);
}

void Shader_2::triangle_debug()
{
    glUseProgram(0);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(0.5f, -0.5f);
    glEnd();
}

void Shader_2::set_uniform_location(const char *name)
{
    int location = glGetUniformLocation(id, name);
    uniformLocationMap[name] = location;
}

int Shader_2::get_uniform_location(const char *name)
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

uint Shader_2::getID()
{
    return id;
}

string Shader_2::debug_vertShaderOut()
{
    // replaceBindings(169);
    cout << vertexCode << endl;
    return vertexCode;
}

string Shader_2::debug_fragShaderOut()
{
    cout << fragmentCode << endl;
    return fragmentCode;
}

void Shader_2::attachSSBO(int binding, shaderTypeEnum shaderType)
{
    std::string toReplace = "binding = 99"; // out of bounds binding max is 80
    std::string replacement = "binding = "+ std::to_string(binding);

    if(shaderType == shaderTypeEnum::vertShader)
    {
        size_t pos = vertexCode.find(toReplace);
        if (pos != std::string::npos) {
            // Replace "binding = 0" with "binding = 1"
            vertexCode.replace(pos, toReplace.length(), replacement);
        }
    }
    if(shaderType == shaderTypeEnum::fragShader)
    {
        size_t pos = fragmentCode.find(toReplace);
        if (pos != std::string::npos) {
            // Replace "binding = 0" with "binding = 1"
            fragmentCode.replace(pos, toReplace.length(), replacement);
        }
    }
}

void Shader_2::attachSSBO(int vsBinding, int fsBinding)
{
    string toReplace = "binding = 99"; // out of bounds binding max is 80
    string replacement = "binding = "+ to_string(vsBinding);

    size_t pos = vertexCode.find(toReplace);
    if (pos != string::npos) 
    {
        vertexCode.replace(pos, toReplace.length(), replacement);
    }
    
    replacement = "binding = "+ to_string(fsBinding);

    pos = fragmentCode.find(toReplace);
    if (pos != string::npos) 
    {
        fragmentCode.replace(pos, toReplace.length(), replacement);
    }
    
}

void Shader_2::linkProgram(bool isCamera)
{
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

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

    if(isCamera)
    {
        // two gl uniforms that will be const in camera shaders
        set_uniform_location("u_view");
        set_uniform_location("u_projection");
    }

    set_uniform_location("u_textureArray");

    // delete the shaders as they're linked into our program now 
    // and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}