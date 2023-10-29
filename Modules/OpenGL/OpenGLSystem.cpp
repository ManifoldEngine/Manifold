#include "OpenGLSystem.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Core/Application.h>
#include <Core/Log.h>
#include <Core/Assert.h>
#include <iostream>
#include <format>
#include <vector>

using namespace ECSEngine;

bool OpenGLSystem::initializeRenderConfig(RendererContext& context)
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

unsigned int OpenGLSystem::createVAO(const std::vector<float>& vertices, int index)
{
    ECSE_ASSERT(vertices.size() > 0, "tried to configure empty vertices");

    const float* verticesArr = &vertices[0];

    // we're going to assign the vertices data to a Vertex Array Object (VAO) for easier access.
    unsigned int vertexArrayObjectId;
    glCreateVertexArrays(1, &vertexArrayObjectId);

    // bind the vertex array object id, it is now the current VAO.
    glBindVertexArray(vertexArrayObjectId);

    // VBO (Vertex buffer objects) manages the vertex memory.
    // it's preferable to batch VBO updates
    // first, we generate a buffer
    unsigned int vertexBufferObjectId;
    glGenBuffers(1, &vertexBufferObjectId);

    // then we find that buffer to a type, in this case a GL_ARRAY_BUFFER
    // one buffer of each type can be bound at one time.
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectId);

    // since the current buffer bound to GL_ARRAY_BUFFER is our VBO, 
    // we copy the vertices data into that buffer.
    // GL_STREAM_DRAW : the data is set only once and used by the GPU at most a few times.
    // GL_STATIC_DRAW : the data is set only once and used many times.
    // GL_DYNAMIC_DRAW : the data is changed a lot and used many times.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), verticesArr, GL_STATIC_DRAW);

    // we're attributing the arguments that will be passed to the vertex shader.
    // the data will be taken from the current buffer bound to GL_ARRAY_BUFFER, which is our VBO
    // containing the vertices data.
    // arguments:
    //  - starting position of the data
    //  - the amount of values (xyz)
    //  - the type of the data
    //  - is the data normalized ?
    //  - the size of the data
    //  - the offset to access the data in the buffer (we start at 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // this enables the VAO that we just added.
    glEnableVertexAttribArray(index);
    
    // unbind the vertex array object to clean up.
    glBindVertexArray(0);

    return vertexArrayObjectId;
}

// glfw callbacks begin
void OpenGLSystem::glfwCallback_onWindowClosed(GLFWwindow* window)
{
    if (auto* openGlSystem = (OpenGLSystem*)glfwGetWindowUserPointer(window))
    {
        openGlSystem->onWindowClosed.broadcast(openGlSystem->window);
        Application::get().stop();
    }
}

void OpenGLSystem::glfwCallback_onWindowResized(GLFWwindow* window, int newWidth, int newHeight)
{
    if (auto* openGlSystem = (OpenGLSystem*)glfwGetWindowUserPointer(window))
    {
        openGlSystem->window.width = newWidth;
        openGlSystem->window.height = newHeight;
    }

    glViewport(0, 0, newWidth, newHeight);
}
// glfw callbacks begin

ECSEngine::OpenGLSystem::OpenGLSystem()
{
    vertexArrayObjectIds = new std::vector<unsigned int>();
}

ECSEngine::OpenGLSystem::~OpenGLSystem()
{
    delete vertexArrayObjectIds;
}

std::string_view OpenGLSystem::getName() const
{
    return "OpenGLSystem";
}

bool OpenGLSystem::shouldTick(EntityRegistry& registry) const
{
    return true;
}

void OpenGLSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    SystemBase::onInitialize(registry, systemContainer);

    // initialize glfw
    if (!glfwInit())
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to init glfw");
        return;
    }

    // set glfw context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef ECSE_MACOSX
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // create the window
    window.m_pWindow = glfwCreateWindow(window.width, window.height, window.name.data(), NULL, NULL);
    if (window.m_pWindow == nullptr)
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to create glfwwindow");
        glfwTerminate();
        return;
    }

    // set this as the window's user pointer. This allows us to retrieve this pointer from the window pointer provided in glfw's callbacks.
    glfwSetWindowUserPointer(window.m_pWindow, this);
    glfwMakeContextCurrent(window.m_pWindow);

    // set glfw callbacks
    glfwSetWindowCloseCallback(window.m_pWindow, &OpenGLSystem::glfwCallback_onWindowClosed);
    glfwSetFramebufferSizeCallback(window.m_pWindow, &OpenGLSystem::glfwCallback_onWindowResized);

    // init glew to load the correct opengl runtime
    GLenum result = glewInit();
    if (result != GLEW_OK)
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to init glew");
        glfwTerminate();
        return;
    }

    // set the view port to the window's size.
    glViewport(0, 0, window.width, window.height);

    if (!initializeRenderConfig(renderer))
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to initialize the render config.");
        glfwTerminate();
        return;
    }

    // triangle vertices data
    std::vector<float> vertices =
    {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };
    const unsigned int vertexArrayObjectId = createVAO(vertices, vertexArrayObjectIds->size());
    vertexArrayObjectIds->push_back(vertexArrayObjectId);
}

void OpenGLSystem::onDeinitialize(EntityRegistry& entityRegistry)
{
    SystemBase::onDeinitialize(entityRegistry);

    glfwTerminate();
    window.m_pWindow = nullptr;
}

void OpenGLSystem::tick(float deltaTime, EntityRegistry& entityRegistry)
{
    if (window.m_pWindow == nullptr)
    {
        return;
    }

    // render pipeline:
    // vertex shader -> geometry shader -> fragment shader -> rasterization
    // vertex shader: translate vertices coordinate to open gl's screen space coordinate.
    //  coordinates that are not -1 >= c >= 1 will be clipped out
    // fragment shader: determine the color of a pixel.
    // vertex shader & fragment shader are required

    // setting color state.
    glClearColor(.2f, .3f, .3f, 1.f);

    // consuming color state.
    glClear(GL_COLOR_BUFFER_BIT);

    // set the shader program to be used.
    glUseProgram(renderer.shaderProgramId);
    for (const auto& vertexArrayObjectId : *vertexArrayObjectIds)
    {
        glBindVertexArray(vertexArrayObjectId);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    glBindVertexArray(0);

    glfwSwapBuffers(window.m_pWindow);
    glfwPollEvents();
}