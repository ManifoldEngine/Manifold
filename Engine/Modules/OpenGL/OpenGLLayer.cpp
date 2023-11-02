#include "OpenGLLayer.h"
#include <Core/Log.h>
#include <GL/glew.h>
#include <Core/Assert.h>

using namespace ECSEngine;

void OpenGLLayer::initialize()
{
    if (!initializeShaderProgram(orangeShaderProgram))
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to initialize the render config.");
        return;
    }
    
    float halfScreenSquareVertices[] =
    {
         0.0f,  1.0f, 0.0f, // top right
         0.0f, -1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f // top left
    };

    float squareVertices[] =
    {
         0.5f,  0.5f, 0.0f, // top right
         0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f // top left
    };

    unsigned int squareIndices[] = {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    m_pHalfScreenSquareVertexArray = std::make_unique<OpenGLVertexArray>();
    std::shared_ptr<OpenGLVertexBuffer> pHalfscreenSquareVertexBuffer = std::make_shared<OpenGLVertexBuffer>(halfScreenSquareVertices, (int)sizeof(squareVertices));
    m_pHalfScreenSquareVertexArray->addVertexBuffer(pHalfscreenSquareVertexBuffer);

    std::shared_ptr<OpenGLIndexBuffer> pIndexBuffer = std::make_shared<OpenGLIndexBuffer>(squareIndices, (int)sizeof(squareIndices));
    m_pHalfScreenSquareVertexArray->setIndexBuffer(pIndexBuffer);

    m_pSquareVertexArray = std::make_unique<OpenGLVertexArray>();
    std::shared_ptr<OpenGLVertexBuffer> pSquareVertexBuffer = std::make_shared<OpenGLVertexBuffer>(squareVertices, (int)sizeof(squareVertices));
    m_pSquareVertexArray->addVertexBuffer(pSquareVertexBuffer);

    m_pSquareVertexArray->setIndexBuffer(pIndexBuffer);
}

void OpenGLLayer::deinitialize()
{
    m_pHalfScreenSquareVertexArray.reset();
    m_pSquareVertexArray.reset();
}

void OpenGLLayer::tick(float deltaTime)
{
    // setting color state.
    glClearColor(.2f, .3f, .3f, 1.f);

    // consuming color state.
    glClear(GL_COLOR_BUFFER_BIT);

    // set the shader program to be used.
    glUseProgram(orangeShaderProgram.shaderProgramId);

    m_pHalfScreenSquareVertexArray->bind();
    if (const auto& pIndexBuffer = m_pHalfScreenSquareVertexArray->getIndexBuffer())
    {
        glDrawElements(GL_TRIANGLES, pIndexBuffer->getStrideCount(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        ECSE_ASSERT(false, "no index buffer provided with the vertices");
    }
}

bool OpenGLLayer::initializeShaderProgram(ShaderProgram& program)
{
    const uint32_t vertexShaderId = compileShader(program.vertexShaderSource, GL_VERTEX_SHADER);
    if (vertexShaderId == UINT32_MAX)
    {
        return false;
    }

    const uint32_t fragmentShaderId = compileShader(program.fragmentShaderSource, GL_FRAGMENT_SHADER);
    if (fragmentShaderId == UINT32_MAX)
    {
        return false;
    }

    program.shaderProgramId = linkShaderProgram(vertexShaderId, fragmentShaderId);
    return program.shaderProgramId != UINT32_MAX;
}

uint32_t OpenGLLayer::compileShader(const std::string_view& source, int shaderType)
{
    const uint32_t shaderId = glCreateShader(shaderType);
    const char* const pSource = source.data();
    glShaderSource(shaderId, 1, &pSource, NULL);
    glCompileShader(shaderId);

    int bIsSuccess = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &bIsSuccess);
    if (!bIsSuccess)
    {
        char infoLog[512];
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        ECSE_LOG_ERROR(LogOpenGL, "Vertex shader compilation failed: {}", infoLog);
        return UINT32_MAX;
    }

    return shaderId;
}

uint32_t OpenGLLayer::linkShaderProgram(uint32_t vertexShaderId, uint32_t fragmentShaderId)
{
    const uint32_t shaderProgramId = glCreateProgram();
    glAttachShader(shaderProgramId, vertexShaderId);
    glAttachShader(shaderProgramId, fragmentShaderId);
    glLinkProgram(shaderProgramId);

    {
        int bIsSuccess = 0;
        glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &bIsSuccess);
        if (!bIsSuccess)
        {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgramId, 512, NULL, infoLog);
            ECSE_LOG_ERROR(LogOpenGL, "program link failed: {}", infoLog);
            return UINT32_MAX;
        }
    }

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    return shaderProgramId;
}
