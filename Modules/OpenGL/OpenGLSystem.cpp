#include "OpenGLSystem.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Core/Application.h>
#include <Core/Log.h>
#include <iostream>

using namespace ECSEngine;

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

    SET_CHANNEL_LOG_LEVEL(LogOpenGL, ELogLevel::Verbose);

    if (!glfwInit())
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to init glfw");
        return;
    }

    m_pWindow = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (m_pWindow == nullptr)
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to create glfwwindow");
        glfwTerminate();
        return;
    }

    // set this as the window's user pointer. This allows us to retrieve this pointer from the window pointer provided in glfw's callbacks.
    glfwSetWindowUserPointer(m_pWindow, this);

    glfwMakeContextCurrent(m_pWindow);

    glfwSetWindowCloseCallback(m_pWindow, [](GLFWwindow* window) 
    {
        if (auto* openGlSystem = (OpenGLSystem*)glfwGetWindowUserPointer(window))
        {
            openGlSystem->onWindowClosed.broadcast();
            Application::get().stop();
        }
    });

    GLenum result = glewInit();
    if (result != GLEW_OK)
    {
        ECSE_LOG_ERROR(LogOpenGL, "failed to init glew");
        return;
    }
}

void OpenGLSystem::onDeinitialize(EntityRegistry& entityRegistry)
{
    SystemBase::onDeinitialize(entityRegistry);

    glfwTerminate();
}

void OpenGLSystem::tick(float deltaTime, EntityRegistry& entityRegistry)
{
    if (m_pWindow == nullptr)
    {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(m_pWindow);
    glfwPollEvents();
}
