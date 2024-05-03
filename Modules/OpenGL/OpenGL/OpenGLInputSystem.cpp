#include "OpenGLInputSystem.h"

#include <Inputs/Data/InputDevice.h>

#include <GLFW/glfw3.h>
#include <OpenGLSystem.h>
#include <algorithm>

using namespace Mani;

// OpenGL callbacks BEGIN
glm::vec2 OpenGLInputSystem::s_mouse = glm::vec2(0.f);

void OpenGLInputSystem::glfwCallback_onMouseMoved(GLFWwindow* window, double x, double y)
{
    s_mouse.x = static_cast<float>(x);
    s_mouse.y = static_cast<float>(y);
}
// OpenGL callbacks END

void Mani::OpenGLInputSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
    m_openGLSystem = Application::get().getSystemContainer().getSystem<OpenGLSystem>();
    MANI_ASSERT(!m_openGLSystem.expired(), "Needs OpenGLSystem to execute.");

    std::shared_ptr<OpenGLSystem> openGLSystem = m_openGLSystem.lock();
    const OpenGLSystem::WindowContext& context = openGLSystem->getWindowContext();
    glfwSetCursorPosCallback(context.window, &OpenGLInputSystem::glfwCallback_onMouseMoved);

    InputDevice* inputDevice = registry.addComponent<InputDevice>(registry.getSingletonId());
    inputDevice->deviceName = "OpenGLInput";
    inputDevice->axis =
    {
        { "WASD" },
        { "Mouse" },
    };
}

void Mani::OpenGLInputSystem::onDeinitialize(EntityRegistry& registry)
{
    if (!m_openGLSystem.expired())
    {
        std::shared_ptr<OpenGLSystem> openGLSystem = m_openGLSystem.lock();
        const OpenGLSystem::WindowContext& context = openGLSystem->getWindowContext();
        glfwSetCursorPosCallback(context.window, NULL);
    }

    registry.removeComponent<InputDevice>(registry.getSingletonId());
}

void Mani::OpenGLInputSystem::tick(float deltaTime, EntityRegistry& registry)
{
    if (m_openGLSystem.expired())
    {
        return;
    }

    std::shared_ptr<OpenGLSystem> openGLSystem = m_openGLSystem.lock();

    InputDevice* inputDevice = registry.getComponent<InputDevice>(registry.getSingletonId());
    if (inputDevice == nullptr)
    {
        return;
    }

    AxisControl& wasd = inputDevice->axis[0];
    wasd.x = 0.f;
    wasd.y = 0.f;
    wasd.z = 0.f;

    AxisControl& mouse = inputDevice->axis[1];
    mouse.x = s_mouse.x;
    mouse.y = s_mouse.y;

    const OpenGLSystem::WindowContext& context = openGLSystem->getWindowContext();
    
    // MOUSE LEFT CLICK
    if (glfwGetKey(context.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "MouseLeftClick", true });
    }
    if (glfwGetKey(context.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "MouseLeftClick", false });
    }

    // W
    if (glfwGetKey(context.window, GLFW_KEY_W) == GLFW_PRESS)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "W", true });
        wasd.z += 1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_W) == GLFW_RELEASE)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "W", false });
    }

    // S
    if (glfwGetKey(context.window, GLFW_KEY_S) == GLFW_PRESS)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "S", true });
        wasd.z += -1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_S) == GLFW_RELEASE)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "S", false });
    }

    // A
    if (glfwGetKey(context.window, GLFW_KEY_A) == GLFW_PRESS)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "A", true });
        wasd.x += 1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_A) == GLFW_RELEASE)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "A", false });
    }

    // D
    if (glfwGetKey(context.window, GLFW_KEY_D) == GLFW_PRESS)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "D", true });
        wasd.x += -1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_D) == GLFW_RELEASE)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "D", false });
    }

    // E
    if (glfwGetKey(context.window, GLFW_KEY_E) == GLFW_PRESS)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "E", true });
        wasd.y += 1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_E) == GLFW_RELEASE)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "E", false });
    }

    // Q
    if (glfwGetKey(context.window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "Q", true });
        wasd.y += -1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_Q) == GLFW_RELEASE)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "Q", false });
    }

    // F
    if (glfwGetKey(context.window, GLFW_KEY_F) == GLFW_PRESS)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "F", true });
    }
    if (glfwGetKey(context.window, GLFW_KEY_F) == GLFW_RELEASE)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "F", false });
    }

    // F7
    if (glfwGetKey(context.window, GLFW_KEY_F7) == GLFW_PRESS)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "F7", true });
    }
    if (glfwGetKey(context.window, GLFW_KEY_F7) == GLFW_RELEASE)
    {
        inputDevice->buttonBuffer.push_back(ButtonControl{ "F7", false });
    }
}
