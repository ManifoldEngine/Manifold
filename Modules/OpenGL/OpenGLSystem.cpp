#include "OpenGLSystem.h"
#include "OpenGLLayer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Core/Application.h>
#include <Core/Log.h>
#include <Core/Assert.h>

#include <iostream>
#include <format>
#include <vector>
#include <memory>

using namespace ECSEngine;

void ECSEngine::OpenGLSystem::destroyWindow(WindowContext& context)
{
    if (context.pWindow != nullptr)
    {
        glfwDestroyWindow(context.pWindow);
        context.pWindow = nullptr;
    }
}

void OpenGLSystem::terminate()
{
    destroyWindow(window);
    glfwTerminate();
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

OpenGLSystem::OpenGLSystem()
{
    m_pLayer = new OpenGLLayer();
}

OpenGLSystem::~OpenGLSystem()
{
    delete m_pLayer;
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
    window.pWindow = glfwCreateWindow(window.width, window.height, window.name.data(), NULL, NULL);
    if (window.pWindow == nullptr)
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to create glfwwindow");
        terminate();
        return;
    }

    glfwGetWindowSize(window.pWindow, &window.width, &window.height);
    
    // set this as the window's user pointer. This allows us to retrieve this pointer from the window pointer provided in glfw's callbacks.
    glfwSetWindowUserPointer(window.pWindow, this);
    glfwMakeContextCurrent(window.pWindow);

    // set glfw callbacks
    glfwSetWindowCloseCallback(window.pWindow, &OpenGLSystem::glfwCallback_onWindowClosed);
    glfwSetFramebufferSizeCallback(window.pWindow, &OpenGLSystem::glfwCallback_onWindowResized);

    // init glew to load the correct opengl runtime
    GLenum result = glewInit();
    if (result != GLEW_OK)
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to init glew");
        terminate();
        return;
    }

    // set the view port to the window's size.
    glViewport(0, 0, window.width, window.height);

    m_pLayer->initialize();
}

void OpenGLSystem::onDeinitialize(EntityRegistry& entityRegistry)
{
    SystemBase::onDeinitialize(entityRegistry);

    m_pLayer->deinitialize();
    terminate();
}

void OpenGLSystem::tick(float deltaTime, EntityRegistry& entityRegistry)
{
    if (window.pWindow == nullptr)
    {
        return;
    }
   
    // render pipeline:
    // vertex shader -> geometry shader -> fragment shader -> rasterization
    // vertex shader: translate vertices coordinate to open gl's screen space coordinate.
    //  coordinates that are not -1 >= c >= 1 will be clipped out
    // fragment shader: determine the color of a pixel.
    // vertex shader & fragment shader are required
    m_pLayer->tick(deltaTime);

    glfwSwapBuffers(window.pWindow);
    glfwPollEvents();
}