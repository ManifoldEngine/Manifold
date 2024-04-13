#include "OpenGLSystem.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <Core/Application.h>
#include <Core/Log.h>
#include <Core/ManiAssert.h>

#include <Camera/CameraSystem.h>

#include <OpenGLDebug.h>

#include <iostream>
#include <vector>
#include <memory>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace Mani;

OpenGLSystem* OpenGLSystem::s_openGLSystem = nullptr;

void OpenGLSystem::destroyWindow(WindowContext& context)
{
    if (context.window != nullptr)
    {
        glfwDestroyWindow(context.window);
        context.window = nullptr;
    }
}

void OpenGLSystem::terminate()
{
    destroyWindow(m_context);
    glfwTerminate();
}

// glfw callbacks begin
void OpenGLSystem::glfwCallback_onWindowClosed(GLFWwindow* window)
{
    if (auto* openGlSystem = (OpenGLSystem*)glfwGetWindowUserPointer(window))
    {
        openGlSystem->onWindowClosed.broadcast(openGlSystem->m_context);
        Application::get().stop();
    }
}

void OpenGLSystem::glfwCallback_onWindowResized(GLFWwindow* window, int newWidth, int newHeight)
{
    if (auto* openGlSystem = (OpenGLSystem*)glfwGetWindowUserPointer(window))
    {
        openGlSystem->m_context.width = newWidth;
        openGlSystem->m_context.height = newHeight;
    }

    glViewport(0, 0, newWidth, newHeight);
}
// glfw callbacks begin

Mani::OpenGLSystem::OpenGLSystem()
{
    // there should be only one OpenGLSystem instance.
    MANI_ASSERT(s_openGLSystem == nullptr, "an OpenGLSystem instance already exists.");
    s_openGLSystem = this;
}

OpenGLSystem& Mani::OpenGLSystem::get()
{
    return *s_openGLSystem;
}

std::string_view OpenGLSystem::getName() const
{
    return "OpenGLSystem";
}

bool OpenGLSystem::shouldTick(EntityRegistry& registry) const
{
    return true;
}

const OpenGLSystem::WindowContext& OpenGLSystem::getWindowContext() const
{
    return m_context;
}

std::shared_ptr<OpenGLInput> Mani::OpenGLSystem::getInputGenerator() const
{
    return m_openGLInputGenerator;
}

void OpenGLSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    // initialize glfw
    if (!glfwInit())
    {
        MANI_LOG_ERROR(LogOpenGL, "failed to init glfw");
        return;
    }

    // set glfw context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef MANI_MACOSX
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // create the window
    m_context.window = glfwCreateWindow(m_context.width, m_context.height, m_context.name.data(), NULL, NULL);
    if (m_context.window == nullptr)
    {
        MANI_LOG_ERROR(LogOpenGL, "failed to create glfwwindow");
        terminate();
        return;
    }

    glfwGetWindowSize(m_context.window, &m_context.width, &m_context.height);
    glfwSetInputMode(m_context.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // set this as the window's user pointer. This allows us to retrieve this pointer from the window pointer provided in glfw's callbacks.
    glfwSetWindowUserPointer(m_context.window, this);
    glfwMakeContextCurrent(m_context.window);

    // set glfw callbacks
    glfwSetWindowCloseCallback(m_context.window, &OpenGLSystem::glfwCallback_onWindowClosed);
    glfwSetFramebufferSizeCallback(m_context.window, &OpenGLSystem::glfwCallback_onWindowResized);

    // init glew to load the correct opengl runtime
    GLenum result = glewInit();
    if (result != GLEW_OK)
    {
        MANI_LOG_ERROR(LogOpenGL, "failed to init glew");
        terminate();
        return;
    }

    // set the view port to the window's size.
    glViewport(0, 0, m_context.width, m_context.height);
    
#ifndef MANI_WEBGL
    #if MANI_OPENGL_DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);
    #endif
#endif

    m_openGLInputGenerator = std::make_shared<OpenGLInput>(shared_from_this());
}

void OpenGLSystem::onDeinitialize(EntityRegistry& entityRegistry)
{
    SystemBase::onDeinitialize(entityRegistry);

    m_openGLInputGenerator.reset();

    terminate();
}

void OpenGLSystem::tick(float deltaTime, EntityRegistry& entityRegistry)
{
    glfwSwapBuffers(m_context.window);
    glfwPollEvents();

#ifdef __EMSCRIPTEN__
    emscripten_sleep(100);
#endif
}