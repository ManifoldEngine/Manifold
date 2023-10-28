#include "OpenGLSystem.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace ECSEngine;

std::string_view OpenGLSystem::getName() const
{
    return "OpenGLSystem";
}

bool OpenGLSystem::shouldTick(EntityRegistry& entityRegistry) const
{
    return true;
}

void OpenGLSystem::onInitialize(EntityRegistry& entityRegistry, SystemContainer& systemContainer)
{
    SystemBase::onInitialize(entityRegistry, systemContainer);

    if (!glfwInit())
    {
        // TODO: implement logging
        std::cout << "failed to init glfw" << std::endl;
        return;
    }

    m_pWindow = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (m_pWindow == nullptr)
    {
        // TODO: implement logging
        std::cout << "failed to create glfwwindow" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_pWindow);

    GLenum result = glewInit();
    if (result != GLEW_OK)
    {
        // TODO: implement logging
        std::cout << "failed to init glew" << std::endl;
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
