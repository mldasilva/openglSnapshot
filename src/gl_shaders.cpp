#include "gl_shaders.h"

// =======================================================================================
//
// Buffer Object
//
// =======================================================================================
bufferObject::bufferObject(vector<vertex>& vertices, uint vertexOffset, vector<uint>& indices, vector<DrawElementsIndirectCommand>& command)
{
    glGenVertexArrays(1, &VAO); 
    glBindVertexArray(VAO); 
    
    glGenBuffers(1, &VBO); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW); 
    
    if(vertexOffset == 3)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexOffset * sizeof(GLfloat), (GLvoid*)0);
    }
    else if(vertexOffset == 8)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texUV));
    }
    else{
        // printf("bufferObject error\n");
        cout << "bufferObject error\n" << endl;
        return;
    }

    glGenBuffers(1, &EBO); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), indices.data(), GL_STATIC_DRAW); 
    

    if(command.data() != NULL)
    {
        glGenBuffers(1, &indirectBuffer); 
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer); 
        glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand) * command.size(), command.data(), GL_STATIC_DRAW);
        // glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        commandCount = command.size();
    }
    else{
        // printf("bufferObject command error\n");
        cout << "bufferObject command error\n" << endl;
        return;
    }

}

// =======================================================================================
//
// Camera
//
// =======================================================================================

camera::camera(uint width, uint height)
{
    zoom = 50.0f;

    float halfWidth = (width / 2.0f) / zoom;
    float halfHeight = (height / 2.0f) / zoom;

    // Orthographic projection setup
    projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.0f, 100.0f);

    // View matrix setup (camera looking at the scene)
    position = glm::vec3(5.0f, 5.0f, 5.0f);   // Camera position
    target = glm::vec3(0.0f, 0.0f, 0.0f); // Look at the origin

    up = glm::vec3(0.0f, 1.0f, 0.0f);           // Up vector
    view = glm::lookAt(position, target, up);
}

void camera::rotate(float angleAddition)
{
    // Calculate the direction vector from the camera position to the target
    glm::vec3 direction = position - target;

    // Step 1: Apply rotation around the Y-axis
    // You can use glm::rotate to create a rotation matrix around the desired axis (e.g., Y-axis)
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angleAddition), up);

    // Step 2: Apply the rotation to the direction vector
    glm::vec3 rotatedDirection = glm::vec3(rotationMatrix * glm::vec4(direction, 1.0f));

    // Step 3: Set the new camera position after rotation
    position = target + rotatedDirection;

    // Step 4: Create the view matrix using the new camera position
    view = glm::lookAt(position, target, up);
}

// =======================================================================================
//
// Shader
//
// =======================================================================================

shader::shader(const char* vertexPath, const char* fragmentPath)
{
    ssbo.resize(1);
    ssbo_map.resize(1);

    // 1. retrieve the vertex/fragment source code from filePath
    string vertexCode;
    string fragmentCode;
    ifstream vShaderFile;
    ifstream fShaderFile;
    
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    try 
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        stringstream vShaderStream, fShaderStream;

        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // close file handlers
        vShaderFile.close();
        fShaderFile.close();

        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (ifstream::failure& e)
    {
        cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << endl;
    }

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

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
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

void shader::draw(camera camera, bufferObject buffer)
{
    // Use the shader program
    glUseProgram(id);

    // Get the location of the 'modelMatrix' uniform in the shader
    GLint viewmatrix = glGetUniformLocation(id, "u_view");
    GLint projectionmatrix = glGetUniformLocation(id, "u_projection");

    // Pass the glm::mat4 to the shader
    glUniformMatrix4fv(viewmatrix, 1, GL_FALSE, glm::value_ptr(camera.view));
    glUniformMatrix4fv(projectionmatrix, 1, GL_FALSE, glm::value_ptr(camera.projection));
    
    glBindVertexArray(buffer.VAO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer.indirectBuffer);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, buffer.commandCount, sizeof(DrawElementsIndirectCommand));
}

//CREATE_SHADER_BUFFER_STORAGE
void shader::create_ssbo(uint index, uint size, const void * data)
{
    glGenBuffers(1, &ssbo[index]); // by address
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[index]);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, data, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ssbo[index]); // by value
    ssbo_map[index] = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
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

// =======================================================================================
//
// Render Pool
//
// =======================================================================================


void renderPool::insert(vector<vertex> v, vector<uint> i, vec3 position)
{
    // rp.commands[i].count           = objects.object[i].indexCount;
    // rp.commands[i].instanceCount   = 1; // objects.object[i][i].instanceCount
    // rp.commands[i].firstIndex      = indexOffset;
    // rp.commands[i].baseVertex      = vertexOffset;
    // rp.commands[i].baseInstance    = instanceOffset;

    // indexOffset     += objects.object[i].indexCount;
    // vertexOffset    += objects.object[i].vertexCount;
    // instanceOffset  += 1;

    commands.push_back(DrawElementsIndirectCommand(i.size(), 1, indices.size(), vertices.size(), count));
    vertices.insert(vertices.end(), v.begin(), v.end());
    indices.insert(indices.end(), i.begin(), i.end());

    count++;

    matrices.push_back(translate(mat4(1.0f), position));
}