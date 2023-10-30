#include "OpenGLLayer.h"
#include <Core/Log.h>
#include <GL/glew.h>
#include <Core/Assert.h>

using namespace ECSEngine;

void OpenGLLayer::initialize()
{
    if (!initializeRenderConfig(rendererContext))
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to initialize the render config.");
        return;
    }
    
    m_pVertexArray = std::make_unique<OpenGLVertexArray>();

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

    std::shared_ptr<OpenGLVertexBuffer> pVertexBuffer = std::make_shared<OpenGLVertexBuffer>(squareVertices, (int)sizeof(squareVertices));
    m_pVertexArray->addVertexBuffer(pVertexBuffer);

    std::shared_ptr<OpenGLIndexBuffer> pIndexBuffer = std::make_shared<OpenGLIndexBuffer>(squareIndices, (int)sizeof(squareIndices));
    m_pVertexArray->setIndexBuffer(pIndexBuffer);
}

void OpenGLLayer::deinitialize()
{
	m_pVertexArray.reset();
}

void OpenGLLayer::tick(float deltaTime)
{
    // setting color state.
    glClearColor(.2f, .3f, .3f, 1.f);

    // consuming color state.
    glClear(GL_COLOR_BUFFER_BIT);

    // set the shader program to be used.
    glUseProgram(rendererContext.shaderProgramId);

    m_pVertexArray->bind();
    if (const auto& pIndexBuffer = m_pVertexArray->getIndexBuffer())
    {
        glDrawElements(GL_TRIANGLES, pIndexBuffer->getStrideCount(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        ECSE_ASSERT(false, "no index buffer provided with the vertices");
    }
}

bool OpenGLLayer::initializeRenderConfig(RendererContext& context)
{
    // create a shader instance
    unsigned int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    // attach the source code to the shader instance
    glShaderSource(vertexShaderId, 1, &context.vertexShaderSource, NULL);
    // compile the shader
    glCompileShader(vertexShaderId);

    // check if the compilation was successful
    {
        int bIsSuccess = 0;
        glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &bIsSuccess);
        if (!bIsSuccess)
        {
            char infoLog[512];
            glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
            ECSE_LOG_ERROR(LogOpenGL, "Vertex shader compilation failed: {}", infoLog);
            return false;
        }
    }

    // create a shader instance
    unsigned int fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    // attach the source code to the shader instance
    glShaderSource(fragmentShaderId, 1, &context.fragmentShaderSource, NULL);
    // compile the shader
    glCompileShader(fragmentShaderId);

    // check if the compilation was successful
    {
        int bIsSuccess = 0;
        glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &bIsSuccess);
        if (!bIsSuccess)
        {
            char infoLog[512];
            glGetShaderInfoLog(fragmentShaderId, 512, NULL, infoLog);
            ECSE_LOG_ERROR(LogOpenGL, "Fragment shader compilation failed: {}", infoLog);
            return false;
        }
    }

    // we now need to link the vertex shader and the fragment shader into a shader program
    // linking means that the output of a link will be the input of the next.
    context.shaderProgramId = glCreateProgram();
    glAttachShader(context.shaderProgramId, vertexShaderId);
    glAttachShader(context.shaderProgramId, fragmentShaderId);
    glLinkProgram(context.shaderProgramId);

    {
        int bIsSuccess = 0;
        glGetProgramiv(context.shaderProgramId, GL_LINK_STATUS, &bIsSuccess);
        if (!bIsSuccess)
        {
            char infoLog[512];
            glGetProgramInfoLog(context.shaderProgramId, 512, NULL, infoLog);
            ECSE_LOG_ERROR(LogOpenGL, "program link failed: {}", infoLog);
            return false;
        }
    }

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    return true;
}
