#include "OpenGLLayer.h"
#include <Core/Log.h>
#include <GL/glew.h>

using namespace ECSEngine;

void OpenGLLayer::initialize()
{
    if (!initializeRenderConfig(rendererContext))
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to initialize the render config.");
        return;
    }
    
    m_pVertexArray = std::make_unique<OpenGLVertexArray>();

    // triangle vertices data
    float vertices1[3*3] =
    {
        -0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
    };

    m_pVertexBuffers.push_back(std::make_shared<OpenGLBuffer>(vertices1, 3*3));
    m_pVertexArray->addVertexBuffer(m_pVertexBuffers.back());
}

void OpenGLLayer::deinitialize()
{
	m_pVertexArray.release();
    m_pVertexBuffers.clear();
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
    glDrawArrays(GL_TRIANGLES, 0, 3);
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
