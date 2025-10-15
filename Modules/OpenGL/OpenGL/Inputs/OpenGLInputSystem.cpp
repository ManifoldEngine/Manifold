#include "OpenGLInputSystem.h"

#include <Core/Debug/Profiling.h>

#include <Inputs/Inputs.h>
#include <Inputs/InputSystem.h>
#include <Inputs/Components/InputDevice.h>

#include <OpenGL/OpenGLWindowContext.h>
#include <OpenGL/Inputs/OpenGLInputs.h>
#include <OpenGL/Inputs/OpenGLInputDevices.h>

#include <ManiMaths/Fwd.h>

#include <GLFW/glfw3.h>

using namespace Mani;

struct OpenGLInputSystem::Storage
{
    ECS::EntityId keyboardId = ECS::INVALID_ID;
    ECS::EntityId mouseId = ECS::INVALID_ID;

    // we don't use bools here because of the specific specialization of 
    // std::vector<bool> which doesn't allow to store a reference of the value since 
    // it returns a proxy object when calling operator[]
    List<uint8_t> previousKeyValues;
    List<uint8_t> previousMouseValues;
};

int toGLFWCursorMode(Cursor::EMode mode)
{
    switch (mode)
    {
        case Cursor::EMode::Hidden:     return GLFW_CURSOR_HIDDEN;
        case Cursor::EMode::Normal:     return GLFW_CURSOR_NORMAL;
        case Cursor::EMode::Disabled:   return GLFW_CURSOR_DISABLED;
        case Cursor::EMode::Captured:   return GLFW_CURSOR_CAPTURED;
        default: return 0;
    }
}

bool readGLFWKeyInput(GLFWwindow* window, EOpenGLKeyCode code)
{
    MANI_ASSERT(window != nullptr, "Given null window");
    const int openGLCode = OpenGLInputsStatics::toOpenGLCode(code);
    return static_cast<bool>(glfwGetKey(window, openGLCode));
}

bool readGLFWMouseInput(GLFWwindow* window, EOpenGLMouseCode code)
{
    MANI_ASSERT(window != nullptr, "Given null window");
    const int openGLCode = OpenGLInputsStatics::toOpenGLCode(code);
    return static_cast<bool>(glfwGetMouseButton(window, openGLCode));
}

Vec2f readGLFWMousePosition(GLFWwindow* window)
{
    double x = 0.0;
    double y = 0.0;
    glfwGetCursorPos(window, &x, &y);
    return Vec2f{
        .x = static_cast<float>(x),
        .y = static_cast<float>(y),
    };
}

template<typename EInputCode, typename TEnumSubType, typename TFunctor = bool(GLFWwindow*, EInputCode)>
void readInputs(GLFWwindow* window, List<uint8_t>& previousValues, const List<ControlId>& controlIds, InputDevice& device, TFunctor&& readOpenGLInput)
{
    for (EInputCode code = EInputCode::First; code < EInputCode::Count; code = static_cast<EInputCode>(static_cast<TEnumSubType>(code) + 1))
    {
        const SizeT index = static_cast<SizeT>(code);

        bool isPressed = readOpenGLInput(window, code);
        uint8_t& previousValue = previousValues[index];
        if (isPressed || previousValue)
        {
            // only notify if something is happening
            ButtonControl button{
                .id = controlIds[index],
                .isPressed = isPressed,
                .hint = OpenGLInputsStatics::toHint(code),
#if MANI_DEBUG
                .debug_name = OpenGLInputsStatics::toString(code),
#endif
            };
            device.buttonBuffer.add(button);
        }

        previousValue = static_cast<uint8_t>(isPressed);
    }
}

template<typename EInputCode, typename TEnumSubType>
void assignControlIdsAndInputHints(ECS::Registry& registry, List<ControlId>& controlIds, InputDevice& device)
{
    for (EInputCode code = EInputCode::First; code < EInputCode::Count; code = static_cast<EInputCode>(static_cast<TEnumSubType>(code) + 1))
    {
        const SizeT index = static_cast<SizeT>(code);
        const ControlId controlId = InputsStatics::generateNextControlId(registry);
        controlIds[index] = controlId;

        const EInputHints hint = OpenGLInputsStatics::toHint(code);
        device.buttonHints[hint] = controlId;
#if MANI_DEBUG
        device.debug_hintTobuttonNames[hint] = OpenGLInputsStatics::toString(code);
#endif
    }
}

void OpenGLInputSystem::onInitialize(ECS::Registry& registry, World& world)
{
    world.initializeDependency<InputSystem>();
    Storage& storage = *registry.addSingle<Storage>();

    {
        //  keyboard
        storage.keyboardId = registry.create();
        InputDevice& device = *registry.add<InputDevice>(storage.keyboardId);
        device.deviceName = "OpenGLKeyboard";
        
        OpenGLKeyboard& keyboard = *registry.add<OpenGLKeyboard>(storage.keyboardId);
        assignControlIdsAndInputHints<EOpenGLKeyCode, uint8_t>(registry, keyboard.keys, device);
    }

    {
        // mouse
        storage.mouseId = registry.create();
        InputDevice& device = *registry.add<InputDevice>(storage.mouseId);
        device.deviceName = "OpenGLMouse";
        AxisControl axis{
            .id = InputsStatics::generateNextControlId(registry),
            .hint = EInputHints::Mouse_Axis,
#if MANI_DEBUG
            .debug_name = "Mouse",
#endif
        };
        device.axis.add(axis);

        OpenGLMouse& mouse = *registry.add<OpenGLMouse>(storage.mouseId);
        assignControlIdsAndInputHints<EOpenGLMouseCode, uint8_t>(registry, mouse.buttons, device);
    }

    storage.previousKeyValues.resize(static_cast<SizeT>(EOpenGLKeyCode::Count));
    storage.previousMouseValues.resize(static_cast<SizeT>(EOpenGLMouseCode::Count));
}

void OpenGLInputSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
    OpenGLWindowContext* context = registry.getSingle<OpenGLWindowContext>();
    MANI_ASSERT(context != nullptr, "We expect the window context to be accessible. If the window is owned by a parent registry, make sure to forward it to this registry.");
    
    const Storage& storage = *registry.getSingle<Storage>();
    registry.destroy(storage.keyboardId);
    registry.destroy(storage.mouseId);
    registry.removeSingle<Storage>();
}

void OpenGLInputSystem::tick(ECS::Registry& registry)
{
    MANI_TIME_SCOPE(OpenGLInputSystem_tick);
    const OpenGLWindowContext& context = *registry.getSingle<OpenGLWindowContext>();

    // handle cursord mode.
    Cursor& cursor = *registry.getSingle<Cursor>();
    glfwSetInputMode(context.window, GLFW_CURSOR, toGLFWCursorMode(cursor.mode));

    glfwPollEvents();

    // read inputs
    Storage& storage = *registry.getSingle<Storage>();
    InputDevice& keyboardDevice = registry.getRef<InputDevice>(storage.keyboardId);
    InputDevice& mouseDevice = registry.getRef<InputDevice>(storage.mouseId);
    OpenGLKeyboard& keyboard = registry.getRef<OpenGLKeyboard>(storage.keyboardId);
    OpenGLMouse& mouse = registry.getRef<OpenGLMouse>(storage.mouseId);

    AxisControl& mouseAxis = mouseDevice.axis.first();
    const Vec2f mousePosition = readGLFWMousePosition(context.window);
    mouseAxis.x = mousePosition.x;
    mouseAxis.y = mousePosition.y;

    readInputs<EOpenGLKeyCode, uint8_t>(context.window, storage.previousKeyValues, keyboard.keys, keyboardDevice, &readGLFWKeyInput);
    readInputs<EOpenGLMouseCode, uint8_t>(context.window, storage.previousMouseValues, mouse.buttons, mouseDevice, &readGLFWMouseInput);
}
