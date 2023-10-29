#include "OpenGLSystem.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Core/Application.h>
#include <Core/Log.h>
#include <iostream>
#include <format>

using namespace ECSEngine;

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

    // init glfw
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

    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window.m_pWindow);
    glfwPollEvents();
}