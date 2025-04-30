#include "OpenGLShader.h"
#include <GL/gl3w.h>
#include <Core/Log.h>
#include <algorithm>
#include <variant>

using namespace Mani;

void OpenGLShader::create(const std::string_view& inName, const std::string_view& inVertexSource, const std::string_view& inFragmentSource)
{
    name = inName;
    vertexSource = inVertexSource;
    fragmentSource = inFragmentSource;
    shaderProgramId = UINT32_MAX;
}

void OpenGLShader::destroy()
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

void OpenGLShader::setShaderType(const std::string_view& key, const ShaderType& value) const
{
    std::visit([&key](const ShaderType& value) {
        using T = std::decay_t<decltype(value)>;

        if         constexpr (std::is_same_v<T, float>) { setFloat(key, std::get<float>(value)); }
        else    if constexpr (std::is_same_v<T, int>)   { setInt(key, std::get<int>(value)); }
        else    if constexpr (std::is_same_v<T, bool>)  { setBool(key, std::get<bool>(value)); }
        else    if constexpr (std::is_same_v<T, Vec2f>) { const Vec2f& v = std::get<Vec2f>(value); setFloat2(key, v.x, v.y); }
        else    if constexpr (std::is_same_v<T, Vec3f>) { const Vec3f& v = std::get<Vec3f>(value); setFloat3(key, v.x, v.y, v.z); }
        else    if constexpr (std::is_same_v<T, Vec4f>) { const Vec4f& v = std::get<Vec4f>(value); setFloat4(key, v.x, v.y, v.z, v.w); }
        else    if constexpr (std::is_same_v<T, Vec2i>) { const Vec2i& v = std::get<Vec2i>(value); setInt2(key, v.x, v.y); }
        else    if constexpr (std::is_same_v<T, Vec3i>) { const Vec3i& v = std::get<Vec3i>(value); setInt3(key, v.x, v.y, v.z); }
        else    if constexpr (std::is_same_v<T, Vec4i>) { const Vec4i& v = std::get<Vec4i>(value); setInt4(key, v.x, v.y, v.z, v.w); }
        else    if constexpr (std::is_same_v<T, Mat3f>) { const Mat3f& v = std::get<Mat3f>(value); setFloatMatrix3(key, &v._00); }
        else    if constexpr (std::is_same_v<T, Mat4f>) { const Mat4f& v = std::get<Mat4f>(value); setFloatMatrix4(key, &v._00); }
    }, value);
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

void OpenGLShader::setFloat(const std::string_view& name, float value) const
{
    const int location = glGetUniformLocation(shaderProgramId, name.data());
    glUniform1f(location, value);
}

void OpenGLShader::setFloat2(const std::string_view& name, float x, float y) const
{
    const int location = glGetUniformLocation(shaderProgramId, name.data());
    glUniform2f(location, x, y);
}

void OpenGLShader::setFloat3(const std::string_view& name, float x, float y, float z) const
{
    const int location = glGetUniformLocation(shaderProgramId, name.data());
    glUniform3f(location, x, y, z);
}

void OpenGLShader::setFloat4(const std::string_view& name, float x, float y, float z, float w) const
{
    const int location = glGetUniformLocation(shaderProgramId, name.data());
    glUniform4f(location, x, y, z, w);
}

void OpenGLShader::setInt(const std::string_view& name, int value) const
{
    const int location = glGetUniformLocation(shaderProgramId, name.data());
    glUniform1i(location, value);
}

void OpenGLShader::setInt2(const std::string_view& name, int x, int y) const
{
    const int location = glGetUniformLocation(shaderProgramId, name.data());
    glUniform2i(location, x, y);
}

void OpenGLShader::setInt3(const std::string_view& name, int x, int y, int z) const
{
    const int location = glGetUniformLocation(shaderProgramId, name.data());
    glUniform3i(location, x, y, z);
}

void OpenGLShader::setInt4(const std::string_view& name, int x, int y, int z, int w) const
{
    const int location = glGetUniformLocation(shaderProgramId, name.data());
    glUniform4i(location, x, y, z, w);
}

void OpenGLShader::setFloatMatrix3(const std::string_view& name, const float* value) const
{
    const int location = glGetUniformLocation(shaderProgramId, name.data());
    glUniformMatrix3fv(location, 1, GL_FALSE, value);
}

void OpenGLShader::setFloatMatrix4(const std::string_view& name, const float* value) const
{
    const int location = glGetUniformLocation(shaderProgramId, name.data());
    glUniformMatrix4fv(location, 1, GL_FALSE, value);
}

void OpenGLShader::setBool(const std::string_view& name, bool value) const
{
    setInt(name, value);
}

void OpenGLShader::setTextureSlot(const std::string_view& name, uint32_t slot) const
{
    setInt(name, slot);
}