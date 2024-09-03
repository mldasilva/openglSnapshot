#ifndef main_h
#define main_h

#include <GL/glew.h>        // must be before glfw
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

using namespace std;

void GLAPIENTRY debugCallback(
    GLenum source, 
    GLenum type, 
    GLuint id, 
    GLenum severity, 
    GLsizei length, 
    const GLchar* message, 
    const void* userParam)
{
    string typeMsg;
    string severityMsg;
    string sourceMsg;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:                       typeMsg = string("GL_DEBUG_TYPE_ERROR"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:         typeMsg = string("GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR");break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:          typeMsg = string("GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR");break;
        case GL_DEBUG_TYPE_PORTABILITY:                 typeMsg = string("GL_DEBUG_TYPE_PORTABILITY");break;
        case GL_DEBUG_TYPE_PERFORMANCE:                 typeMsg = string("GL_DEBUG_TYPE_PERFORMANCE");break;
        case GL_DEBUG_TYPE_OTHER:                       typeMsg = string("GL_DEBUG_TYPE_OTHER");break;
        case GL_DEBUG_TYPE_MARKER:                      typeMsg = string("GL_DEBUG_TYPE_MARKER");break;
        case GL_DEBUG_TYPE_PUSH_GROUP:                  typeMsg = string("GL_DEBUG_TYPE_PUSH_GROUP");break;
        case GL_DEBUG_TYPE_POP_GROUP:                   typeMsg = string("GL_DEBUG_TYPE_POP_GROUP");break;
        default:                                        typeMsg = string("UNKNOWN ERROR");break;
    }
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_NOTIFICATION:            severityMsg = string("GL_DEBUG_SEVERITY_NOTIFICATION");break;
        case GL_DEBUG_LOGGED_MESSAGES:                  severityMsg = string("GL_DEBUG_LOGGED_MESSAGES");break;
        case GL_DEBUG_SEVERITY_HIGH:                    severityMsg = string("GL_DEBUG_SEVERITY_HIGH");break;
        case GL_DEBUG_SEVERITY_MEDIUM:                  severityMsg = string("GL_DEBUG_SEVERITY_MEDIUM");break;
        case GL_DEBUG_SEVERITY_LOW:                     severityMsg = string("GL_DEBUG_SEVERITY_LOW");break;
        default:                                        severityMsg = string("UNKNOWN SEVERITY");break;
    }

    switch (source)
    {   
        case GL_DEBUG_SOURCE_API:                       sourceMsg = string("GL_DEBUG_SOURCE_API");break;                   
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:             sourceMsg = string("GL_DEBUG_SOURCE_WINDOW_SYSTEM");break;      
        case GL_DEBUG_SOURCE_SHADER_COMPILER:           sourceMsg = string("GL_DEBUG_SOURCE_SHADER_COMPILER");break;    
        case GL_DEBUG_SOURCE_THIRD_PARTY:               sourceMsg = string("GL_DEBUG_SOURCE_THIRD_PARTY");break;    
        case GL_DEBUG_SOURCE_APPLICATION:               sourceMsg = string("GL_DEBUG_SOURCE_APPLICATION");break;    
        case GL_DEBUG_SOURCE_OTHER:                     sourceMsg = string("GL_DEBUG_SOURCE_OTHER");break;    
        default:                                        sourceMsg = string("UNKNOWN SOURCE");break; 
    }

    
    // int line = *((int*)userParam);
    // const char* filename = (const char*)userParam;
    
    // printf("%-40s %-40s %-40s %40s\n", "-Source-", "-Type-", "-Severity-", "-Message-");
    // printf("Source: %-30s Type: %-30s Severity: %-30s Message: %40s\n", sourceMsg.c_str(), typeMsg.c_str(), severityMsg.c_str(), message);
    cout << "Source:    " << sourceMsg << endl;
    cout << "Type:      " << typeMsg << endl;
    cout << "Severity:  " << severityMsg << endl;
    cout << "Message:   " << message << endl;

    if(severity == GL_DEBUG_SEVERITY_HIGH)
    {
        printf("\nExiting error severity reached high..\n\n");
        exit(EXIT_FAILURE);
    }
}

void init_glfw_debugger(GLFWwindow *window)
{
    // Check if the OpenGL debug context is enabled
    int debugContext = glfwGetWindowAttrib(window, GLFW_OPENGL_DEBUG_CONTEXT);
    if (debugContext == GLFW_TRUE) 
    {
        printf("OpenGL debug context is enabled.\n\n");
        // Enable debug output
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        // Set the callback function
        // int line = __LINE__;
        // const char* filename = __FILE__;+
        glDebugMessageCallback(debugCallback,  NULL);
        // Filter debug output,
        glDebugMessageControl(  GL_DONT_CARE,                       // source
                                GL_DONT_CARE,                       // type
                                GL_DEBUG_SEVERITY_NOTIFICATION,     // severity
                                0, NULL, 
                                GL_FALSE // true filter in, false filter out
                            );
    } 
    else 
    {
        printf("OpenGL debug context is not enabled.\n");
    }
}

void glfw_hints()
{
    // the compatibility opengl profile makes vao object 0 a default object.
    // the core opengl profile makes VAO object 0 not an object at all. 
    // Core profile we will have to generate a VAO object and bind (glGenVertexArrays glBindVertexArray)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); 
    // GLFW_OPENGL_COMPAT_PROFILE || GLFW_OPENGL_CORE_PROFILE
    //glfwWindowHint(GLFW_RESIZABLE,GL_FALSE); // resizable
    //glfwWindowHint( GLFW_DECORATED, GLFW_FALSE); // bordless

    // DEBUG CONTEXT ENABLE
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1); // 1 on 0 off

    // MSAA
    // glfwWindowHint(GLFW_SAMPLES, 4);
    // glEnable(GL_MULTISAMPLE);

    // gamma correction
    glEnable(GL_FRAMEBUFFER_SRGB); 
}
#endif