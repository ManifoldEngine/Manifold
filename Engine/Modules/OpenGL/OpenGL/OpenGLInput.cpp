#include "OpenGLInput.h"
#include <GLFW/glfw3.h>
#include <OpenGLSystem.h>
#include <algorithm>

using namespace Mani;

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

OpenGLInput::OpenGLInput(const std::weak_ptr<OpenGLSystem>& openGLSystem)
    : m_openGLSystem(openGLSystem)
{
    if (!m_openGLSystem.expired())
    {
        std::shared_ptr<OpenGLSystem> openGLSystem = m_openGLSystem.lock();
        const OpenGLSystem::WindowContext& context = openGLSystem->getWindowContext();
        glfwSetCursorPosCallback(context.window, &OpenGLInput::glfwCallback_onMouseMoved);
    }
}

OpenGLInput::~OpenGLInput()
{
    if (!m_openGLSystem.expired())
    {
        std::shared_ptr<OpenGLSystem> openGLSystem = m_openGLSystem.lock();
        const OpenGLSystem::WindowContext& context = openGLSystem->getWindowContext();
        glfwSetCursorPosCallback(context.window, NULL);
    }
}

std::string OpenGLInput::getName() const
{
    return "OpenGLInputs";
}

bool OpenGLInput::consumeInputBuffer(std::vector<ButtonControl>& outBuffer)
{
    for (const ButtonControl& buttonControl : m_inputBuffer)
    {
        outBuffer.push_back(ButtonControl(buttonControl));
    }
    
    m_inputBuffer.clear();
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
    if (m_openGLSystem.expired())
    {
        return;
    }

    std::shared_ptr<OpenGLSystem> openGLSystem = m_openGLSystem.lock();

    wasd.x = 0.f;
    wasd.y = 0.f;
    wasd.z = 0.f;

    // W
    const OpenGLSystem::WindowContext& context = openGLSystem->getWindowContext();
    if (glfwGetKey(context.window, GLFW_KEY_W) == GLFW_PRESS)
    {
        m_inputBuffer.push_back(ButtonControl{ "W", true });
        wasd.z += 1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_W) == GLFW_RELEASE)
    {
        m_inputBuffer.push_back(ButtonControl{ "W", false });
    }

    // S
    if (glfwGetKey(context.window, GLFW_KEY_S) == GLFW_PRESS)
    {
        m_inputBuffer.push_back(ButtonControl{ "S", true });
        wasd.z += -1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_S) == GLFW_RELEASE)
    {
        m_inputBuffer.push_back(ButtonControl{ "S", false });
    }

    // A
    if (glfwGetKey(context.window, GLFW_KEY_A) == GLFW_PRESS)
    {
        m_inputBuffer.push_back(ButtonControl{ "A", true });
        wasd.x += 1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_A) == GLFW_RELEASE)
    {
        m_inputBuffer.push_back(ButtonControl{ "A", false });
    }

    // D
    if (glfwGetKey(context.window, GLFW_KEY_D) == GLFW_PRESS)
    {
        m_inputBuffer.push_back(ButtonControl{ "D", true });
        wasd.x += -1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_D) == GLFW_RELEASE)
    {
        m_inputBuffer.push_back(ButtonControl{ "D", false });
    }

    // E
    if (glfwGetKey(context.window, GLFW_KEY_E) == GLFW_PRESS)
    {
        m_inputBuffer.push_back(ButtonControl{ "E", true });
        wasd.y += 1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_E) == GLFW_RELEASE)
    {
        m_inputBuffer.push_back(ButtonControl{ "E", false });
    }

    // Q
    if (glfwGetKey(context.window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        m_inputBuffer.push_back(ButtonControl{ "Q", true });
        wasd.y += -1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_Q) == GLFW_RELEASE)
    {
        m_inputBuffer.push_back(ButtonControl{ "Q", false });
    }

    // F
    if (glfwGetKey(context.window, GLFW_KEY_F) == GLFW_PRESS)
    {
        m_inputBuffer.push_back(ButtonControl{ "F", true });
    }
    if (glfwGetKey(context.window, GLFW_KEY_F) == GLFW_RELEASE)
    {
        m_inputBuffer.push_back(ButtonControl{ "F", false });
    }

    // F7
    if (glfwGetKey(context.window, GLFW_KEY_F7) == GLFW_PRESS)
    {
        m_inputBuffer.push_back(ButtonControl{ "F7", true });
    }
    if (glfwGetKey(context.window, GLFW_KEY_F7) == GLFW_RELEASE)
    {
        m_inputBuffer.push_back(ButtonControl{ "F7", false });
    }
}
