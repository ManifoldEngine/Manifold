#include "OpenGLInputSystem.h"

#include <Inputs/InputSystem.h>
#include <Inputs/Data/InputDevice.h>

#include <GLFW/glfw3.h>
#include <OpenGLWindowContext.h>
#include <ManiMaths/Fwd.h>

using namespace Mani;

Vec2f OpenGLInputSystem::s_mouse = VEC2F::ZERO;

void Mani::OpenGLInputSystem::onInitialize(ECS::Registry& registry, SystemContainer& systemContainer)
{
    systemContainer.initializeDependency<InputSystem>();

    OpenGLWindowContext* context = registry.getSingle<OpenGLWindowContext>();
    MANI_ASSERT(context != nullptr, "We expect the window context to be accessible. If the window is owned by a parent registry, make sure to forward it to this registry.");
    glfwSetCursorPosCallback(context->window, &OpenGLInputSystem::glfwCallback_onMouseMoved);

    InputDevice* inputDevice = registry.addSingle<InputDevice>();
    inputDevice->deviceName = "OpenGLInput";
    inputDevice->axis = {
        { "WASD" },
        { "Mouse" },
    };
}

void Mani::OpenGLInputSystem::onDeinitialize(ECS::Registry& registry)
{
    OpenGLWindowContext* context = registry.getSingle<OpenGLWindowContext>();
    MANI_ASSERT(context != nullptr, "We expect the window context to be accessible. If the window is owned by a parent registry, make sure to forward it to this registry.");
    glfwSetCursorPosCallback(context->window, NULL);

    registry.removeSingle<InputDevice>();
}

void Mani::OpenGLInputSystem::tick(float deltaTime, ECS::Registry& registry)
{
    glfwPollEvents();

    InputDevice& inputDevice = *registry.getSingle<InputDevice>();

    AxisControl& wasd = inputDevice.axis[0];
    wasd.x = 0.f;
    wasd.y = 0.f;
    wasd.z = 0.f;

    AxisControl& mouse = inputDevice.axis[1];
    mouse.x = s_mouse.x;
    mouse.y = s_mouse.y;


    const OpenGLWindowContext& context = *registry.getSingle<OpenGLWindowContext>();
    
    {
        // handle cursord mode.
        Cursor& cursor = *registry.getSingle<Cursor>();
        glfwSetInputMode(context.window, GLFW_CURSOR, maniToGLFWCursorMode(cursor.mode));
    }

    // W
    if (glfwGetKey(context.window, GLFW_KEY_W) == GLFW_PRESS)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "W", true });
        wasd.z += 1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_W) == GLFW_RELEASE)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "W", false });
    }

    // S
    if (glfwGetKey(context.window, GLFW_KEY_S) == GLFW_PRESS)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "S", true });
        wasd.z += -1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_S) == GLFW_RELEASE)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "S", false });
    }

    // A
    if (glfwGetKey(context.window, GLFW_KEY_A) == GLFW_PRESS)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "A", true });
        wasd.x += 1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_A) == GLFW_RELEASE)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "A", false });
    }

    // D
    if (glfwGetKey(context.window, GLFW_KEY_D) == GLFW_PRESS)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "D", true });
        wasd.x += -1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_D) == GLFW_RELEASE)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "D", false });
    }

    // E
    if (glfwGetKey(context.window, GLFW_KEY_E) == GLFW_PRESS)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "E", true });
        wasd.y += 1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_E) == GLFW_RELEASE)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "E", false });
    }

    // Q
    if (glfwGetKey(context.window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "Q", true });
        wasd.y += -1.0f;
    }
    if (glfwGetKey(context.window, GLFW_KEY_Q) == GLFW_RELEASE)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "Q", false });
    }

    // F
    if (glfwGetKey(context.window, GLFW_KEY_F) == GLFW_PRESS)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "F", true });
    }
    if (glfwGetKey(context.window, GLFW_KEY_F) == GLFW_RELEASE)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "F", false });
    }

    // F7
    if (glfwGetKey(context.window, GLFW_KEY_F7) == GLFW_PRESS)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "F7", true });
    }
    if (glfwGetKey(context.window, GLFW_KEY_F7) == GLFW_RELEASE)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "F7", false });
    }

    // SPACE
    if (glfwGetKey(context.window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "Space", true });
    }
    if (glfwGetKey(context.window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        inputDevice.buttonBuffer.emplace_back(ButtonControl{ "Space", false });
    }
}

int Mani::OpenGLInputSystem::maniToGLFWCursorMode(Cursor::EMode mode)
{
    switch (mode)
    {
        case Cursor::EMode::HIDDEN:
            return GLFW_CURSOR_HIDDEN;
        case Cursor::EMode::NORMAL:
            return GLFW_CURSOR_NORMAL;
        case Cursor::EMode::DISABLED:
            return GLFW_CURSOR_DISABLED;
        case Cursor::EMode::CAPTURED:
            return GLFW_CURSOR_CAPTURED;
        default:
            return 0;
    }
}

// OpenGL callbacks BEGIN
void OpenGLInputSystem::glfwCallback_onMouseMoved(GLFWwindow* window, double x, double y)
{
    s_mouse.x = static_cast<float>(x);
    s_mouse.y = static_cast<float>(y);
}
// OpenGL callbacks END
