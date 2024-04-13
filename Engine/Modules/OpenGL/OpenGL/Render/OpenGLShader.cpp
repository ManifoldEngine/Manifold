#include "OpenGLShader.h"
#include <GL/glew.h>
#include <Core/Log.h>

using namespace Mani;

OpenGLShader::OpenGLShader(const std::string_view& inName, const std::string_view& inVertexSource, const std::string_view& inFragmentSource)
    : name(inName), vertexSource(inVertexSource), fragmentSource(inFragmentSource), shaderProgramId(UINT32_MAX)
{
}

OpenGLShader::~OpenGLShader()
{
    if (isCompiled())
    {
        glDeleteProgram(shaderProgramId);
    }
}

bool OpenGLShader::compile()
{
    if (isCompiled())
    {
        return true;
    }

    uint32_t vertexShaderId = compile(vertexSource, GL_VERTEX_SHADER);
    if (vertexShaderId == UINT32_MAX)
    {
        return false;
    }

    
    uint32_t fragmentShaderId = compile(fragmentSource, GL_FRAGMENT_SHADER);
    if (fragmentShaderId == UINT32_MAX)
    {
        glDeleteShader(vertexShaderId);
        return false;
    }

    shaderProgramId = glCreateProgram();
    glAttachShader(shaderProgramId, vertexShaderId);
    glAttachShader(shaderProgramId, fragmentShaderId);
    glLinkProgram(shaderProgramId);

    int isSuccess = 0;
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &isSuccess);
    if (!isSuccess)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgramId, 512, NULL, infoLog);
        MANI_LOG_ERROR(LogOpenGL, "program link failed: {}", infoLog);
        shaderProgramId = UINT32_MAX;
        return false;
    }

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    return true;
}

bool OpenGLShader::isCompiled() const
{
    return shaderProgramId != UINT32_MAX;
}

void OpenGLShader::use() const
{
    glUseProgram(shaderProgramId);
}

#include <iostream>

uint32_t OpenGLShader::compile(const std::string_view& inSource, int shaderType)
{
    uint32_t id = glCreateShader(shaderType);
    const char* const source = inSource.data();
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    int isSuccess = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &isSuccess);
    if (!isSuccess)
    {
        char infoLog[512];
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cout << "shader compilation failed: " << infoLog << std::endl;
        MANI_LOG_ERROR(LogOpenGL, "shader compilation failed: {}", infoLog);
        id = UINT32_MAX;
        return id;
    }
    return id;
}

void OpenGLShader::setFloat(const char* name, float value)
{
    const int location = glGetUniformLocation(shaderProgramId, name);
    glUniform1f(location, value);
}

void OpenGLShader::setFloat2(const char* name, float x, float y)
{
    const int location = glGetUniformLocation(shaderProgramId, name);
    glUniform2f(location, x, y);
}

void OpenGLShader::setFloat3(const char* name, float x, float y, float z)
{
    const int location = glGetUniformLocation(shaderProgramId, name);
    glUniform3f(location, x, y, z);
}

void OpenGLShader::setFloat4(const char* name, float x, float y, float z, float w)
{
    const int location = glGetUniformLocation(shaderProgramId, name);
    glUniform4f(location, x, y, z, w);
}

void OpenGLShader::setInt(const char* name, int value)
{
    const int location = glGetUniformLocation(shaderProgramId, name);
    glUniform1i(location, value);
}

void OpenGLShader::setInt2(const char* name, int x, int y)
{
    const int location = glGetUniformLocation(shaderProgramId, name);
    glUniform2i(location, x, y);
}

void OpenGLShader::setInt3(const char* name, int x, int y, int z)
{
    const int location = glGetUniformLocation(shaderProgramId, name);
    glUniform3i(location, x, y, z);
}

void OpenGLShader::setInt4(const char* name, int x, int y, int z, int w)
{
    const int location = glGetUniformLocation(shaderProgramId, name);
    glUniform4i(location, x, y, z, w);
}

void OpenGLShader::setFloatMatrix3(const char* name, float* value)
{
    const int location = glGetUniformLocation(shaderProgramId, name);
    glUniformMatrix3fv(location, 1, GL_FALSE, value);
}

void OpenGLShader::setFloatMatrix4(const char* name, float* value)
{
    const int location = glGetUniformLocation(shaderProgramId, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, value);
}

void OpenGLShader::setBool(const char* name, bool value)
{
    setInt(name, value);
}

void OpenGLShader::setTextureSlot(const char* name, uint32_t slot)
{
    setInt(name, slot);
}