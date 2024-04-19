#include "OpenGLDebug.h"
#ifndef MANI_WEBGL
#include <Core/Log.h>
#include <string_view>
#include <sstream>


void Mani::OpenGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    std::stringstream ss;
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH: ss << "[SEVERITY_HIGH]"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: ss << "[SEVERITY_MEDIUM]"; break;
        case GL_DEBUG_SEVERITY_LOW: ss << "[SEVERITY_LOW]"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: ss << "[SEVERITY_NOTIFICATION]"; break;
        default: break;
    }

    switch (source)
    {
        case GL_DEBUG_SOURCE_API: ss << "[API]"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: ss << "[WINDOW_SYSTEM]"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: ss << "[SHADER_COMPILER]"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: ss << "[THIRD_PARTY]"; break;
        case GL_DEBUG_SOURCE_OTHER: ss << "[OTHER]"; break;
        case GL_DEBUG_SOURCE_APPLICATION: ss << "[APPLICATION]"; break;
        default: break;
    }

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR: ss << "[ERROR]"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: ss << "[DEPRECATED_BEHAVIOR]"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: ss << "[UNDEFINED_BEHAVIOR]"; break;
        case GL_DEBUG_TYPE_PORTABILITY: ss << "[PORTABILITY]"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: ss << "[PERFORMANCE]"; break;
        case GL_DEBUG_TYPE_MARKER: ss << "[MARKER]"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: ss << "[PUSH_GROUP]"; break;
        case GL_DEBUG_TYPE_POP_GROUP: ss << "[POP_GROUP]"; break;
        case GL_DEBUG_TYPE_OTHER: ss << "[OTHER]"; break;
        default: break;
    }

    ss << " " << message;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
        {
            MANI_LOG_ERROR(LogOpenGLAPI, "{}", ss.str());
            break;
        }

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        case GL_DEBUG_TYPE_PORTABILITY:
        case GL_DEBUG_TYPE_PERFORMANCE:
        {
            MANI_LOG_WARNING(LogOpenGLAPI, "{}", ss.str());
            break;
        }

        default:
        {
            MANI_LOG(LogOpenGLAPI, "{}", ss.str());
            break;
        }
    }
}
#endif