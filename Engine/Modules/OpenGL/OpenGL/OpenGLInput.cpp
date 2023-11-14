#include "OpenGLInput.h"
#include <GLFW/glfw3.h>
#include <OpenGLSystem.h>
#include <algorithm>

using namespace ECSEngine;

// OpenGL callbacks BEGIN
void OpenGLInput::glfwCallback_onMouseMoved(GLFWwindow* window, double x, double y)
{
    if (auto* openGLSystem = (OpenGLSystem*)glfwGetWindowUserPointer(window))
    {
        std::shared_ptr<OpenGLInput> openGLInput = openGLSystem->getInputGenerator();
        if (openGLInput == nullptr)
        {
            return;
        }

        openGLInput->mouse.x = x;
        openGLInput->mouse.y = y;
    }
}
// OpenGL callbacks END

void OpenGLInput::initialize(const std::shared_ptr<OpenGLSystem>& openGLSystem)
{
    m_openGLSystem = openGLSystem;

    const OpenGLSystem::WindowContext& context = m_openGLSystem->getWindowContext();
    glfwSetCursorPosCallback(context.window, &OpenGLInput::glfwCallback_onMouseMoved);
}

void OpenGLInput::deinitialize()
{
    if (m_openGLSystem != nullptr)
    {
        const OpenGLSystem::WindowContext& context = m_openGLSystem->getWindowContext();
        glfwSetCursorPosCallback(context.window, NULL);
    }
}

std::string OpenGLInput::getName() const
{
    return "OpenGLInputs";
}

bool OpenGLInput::consumeInputBuffer(std::vector<ButtonControl>& outBuffer)
{
    return true;
}

bool OpenGLInput::getAxis(std::vector<AxisControl>& outAxis)
{
    outAxis.push_back(wasd);
    outAxis.push_back(mouse);
    return true;
}

void OpenGLInput::onInputSystemTick(float deltaTime, EntityRegistry& registry)
{
    if (m_openGLSystem == nullptr)
    {
        return;
    }

    wasd.x = 0.f;
    wasd.y = 0.f;
    wasd.z = 0.f;

    // W
    const OpenGLSystem::WindowContext& context = m_openGLSystem->getWindowContext();
    if (glfwGetKey(context.window, GLFW_KEY_W) == GLFW_PRESS)
    {
        inputBuffer.push_back(ButtonControl{ "W", true });
        wasd.z += 1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_W) == GLFW_RELEASE)
    {
        inputBuffer.push_back(ButtonControl{ "W", false });
    }

    // S
    if (glfwGetKey(context.window, GLFW_KEY_S) == GLFW_PRESS)
    {
        inputBuffer.push_back(ButtonControl{ "S", true });
        wasd.z += -1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_S) == GLFW_RELEASE)
    {
        inputBuffer.push_back(ButtonControl{ "S", false });
    }

    // A
    if (glfwGetKey(context.window, GLFW_KEY_A) == GLFW_PRESS)
    {
        inputBuffer.push_back(ButtonControl{ "A", true });
        wasd.x += 1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_A) == GLFW_RELEASE)
    {
        inputBuffer.push_back(ButtonControl{ "A", false });
    }

    // D
    if (glfwGetKey(context.window, GLFW_KEY_D) == GLFW_PRESS)
    {
        inputBuffer.push_back(ButtonControl{ "D", true });
        wasd.x += -1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_D) == GLFW_RELEASE)
    {
        inputBuffer.push_back(ButtonControl{ "D", false });
    }

    // E
    if (glfwGetKey(context.window, GLFW_KEY_E) == GLFW_PRESS)
    {
        inputBuffer.push_back(ButtonControl{ "E", true });
        wasd.y += 1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_E) == GLFW_RELEASE)
    {
        inputBuffer.push_back(ButtonControl{ "E", false });
    }

    // Q
    if (glfwGetKey(context.window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        inputBuffer.push_back(ButtonControl{ "Q", true });
        wasd.y += -1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_Q) == GLFW_RELEASE)
    {
        inputBuffer.push_back(ButtonControl{ "Q", false });
    }
}
