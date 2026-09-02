#include "OpenGLInputSystem.h"

#include <Core/Profiling/Profiling.h>

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
    EntityId keyboardId = INVALID_ID;
    EntityId mouseId = INVALID_ID;

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
    const int openGLCode = OpenGLInputs::toOpenGLCode(code);
    return static_cast<bool>(glfwGetKey(window, openGLCode));
}

bool readGLFWMouseInput(GLFWwindow* window, EOpenGLMouseCode code)
{
    MANI_ASSERT(window != nullptr, "Given null window");
    const int openGLCode = OpenGLInputs::toOpenGLCode(code);
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
                .hint = OpenGLInputs::toHint(code),
#if MANI_DEBUG
                .debug_name = OpenGLInputs::toString(code),
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
        const ControlId controlId = Inputs::generateNextControlId(registry);
        controlIds[index] = controlId;

        const EInputHints hint = OpenGLInputs::toHint(code);
        device.buttonHints[hint] = controlId;
#if MANI_DEBUG
        device.debug_hintTobuttonNames[hint] = OpenGLInputs::toString(code);
#endif
    }
}

void OpenGLInputSystem::onInitialize(ECS::Registry& registry, World& world)
{
    world.initializeDependency<InputSystem>();
    
    EntityId keyboardId = INVALID_ID;
    EntityId mouseId = INVALID_ID;

    {
        //  keyboard
        keyboardId = registry.create();
        auto [device, keyboard] = registry.addMany<InputDevice, OpenGLKeyboard>(keyboardId);
        
        device->deviceName = "OpenGLKeyboard";
        assignControlIdsAndInputHints<EOpenGLKeyCode, uint8_t>(registry, keyboard->keys, *device);
    }

    {
        // mouse
        mouseId = registry.create();
        auto [device, mouse] = registry.addMany<InputDevice, OpenGLMouse>(mouseId);
        device->deviceName = "OpenGLMouse";
        device->axis.add(AxisControl{
            .id = Inputs::generateNextControlId(registry),
            .hint = EInputHints::Mouse_Axis,
#if MANI_DEBUG
            .debug_name = "Mouse",
#endif
        });

        assignControlIdsAndInputHints<EOpenGLMouseCode, uint8_t>(registry, mouse->buttons, *device);
    }

    Ref<Storage> storage = registry.addSingle<Storage>();
    storage->keyboardId = keyboardId;
    storage->mouseId = mouseId;
    storage->previousKeyValues.resize(static_cast<SizeT>(EOpenGLKeyCode::Count));
    storage->previousMouseValues.resize(static_cast<SizeT>(EOpenGLMouseCode::Count));
}

void OpenGLInputSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
    Ref<Storage> storage = registry.getSingle<Storage>();

    registry.destroy(storage->keyboardId);
    registry.destroy(storage->mouseId);
    registry.removeSingle<Storage>();
}

void OpenGLInputSystem::tick(ECS::Registry& registry)
{
    OpenGLWindowContext& context = registry.getSinglePinned<OpenGLWindowContext>();

    // handle cursord mode.
    Ref<Cursor> cursor = registry.getSingle<Cursor>();
    glfwSetInputMode(context.window, GLFW_CURSOR, toGLFWCursorMode(cursor->mode));

    glfwPollEvents();

    // read inputs
    Ref<Storage> storage = registry.getSingle<Storage>();
    Ref<InputDevice> keyboardDevice = registry.get<InputDevice>(storage->keyboardId);
    Ref<InputDevice> mouseDevice = registry.get<InputDevice>(storage->mouseId);
    Ref<OpenGLKeyboard> keyboard = registry.get<OpenGLKeyboard>(storage->keyboardId);
    Ref<OpenGLMouse> mouse = registry.get<OpenGLMouse>(storage->mouseId);

    AxisControl& mouseAxis = mouseDevice->axis.first();
    const Vec2f mousePosition = readGLFWMousePosition(context.window);
    mouseAxis.x = mousePosition.x;
    mouseAxis.y = mousePosition.y;

    readInputs<EOpenGLKeyCode, uint8_t>(context.window, storage->previousKeyValues, keyboard->keys, *keyboardDevice, &readGLFWKeyInput);
    readInputs<EOpenGLMouseCode, uint8_t>(context.window, storage->previousMouseValues, mouse->buttons, *mouseDevice, &readGLFWMouseInput);
}
