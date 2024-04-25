#include "InputSystem.h"

#include <Inputs/Inputs.h>

#include <Inputs/Data/InputUser.h>
#include <Inputs/Data/InputDevice.h>

using namespace Mani;

void InputSystem::tick(float deltaTime, EntityRegistry& registry)
{
	RegistryView<InputUser> inputUserView(registry);
	for (const EntityId entityId : inputUserView)
	{
		InputUser* inputUser = registry.getComponent<InputUser>(entityId);
		
		// axis are reset each tick.
		for (auto& [name, action] : inputUser->actions)
		{
			action.x = 0.f;
			action.y = 0.f;
			action.z = 0.f;
		}

		// consume assigned input device
		for (const EntityId deviceId : inputUser->inputDevices)
		{
			InputDevice* inputDevice = registry.getComponent<InputDevice>(deviceId);
			if (inputDevice == nullptr)
			{
				continue;
			}

			// buttons
			for (const ButtonControl& control : inputDevice->buttonBuffer)
			{
				auto boundActionNamesIt = inputUser->bindings.find(control.name);
				if (boundActionNamesIt == inputUser->bindings.end())
				{
					continue;
				}

				for (const std::string& actionName : boundActionNamesIt->second)
				{
					InputAction& action = inputUser->actions[actionName];
					if (action.isPressed != control.isPressed)
					{
						MANI_LOG_VERBOSE(LogInputs, "Action {} state changed to {}", action.name, action.isPressed);
						action.isPressed = control.isPressed;
						onActionEvent.broadcast(entityId, action);
					}
				}
			}

			// axis
			for (const AxisControl& axis : inputDevice->axis)
			{
				auto boundActionNamesIt = inputUser->bindings.find(axis.name);
				if (boundActionNamesIt == inputUser->bindings.end())
				{
					continue;
				}

				for (const std::string& actionName : boundActionNamesIt->second)
				{
					InputAction& action = inputUser->actions[actionName];

					action.x += axis.x;
					action.y += axis.y;
					action.z += axis.z;
				}
			}
		}

#if MANI_DEBUG
		for (auto& [name, action] : inputUser->actions)
		{
			// log action state
			MANI_LOG_VERBOSE(LogInputs, "Action {} axis changed to ({}, {}, {})", action.name, action.x, action.y, action.z);
		}
#endif
	}

	// clear button buffers
	RegistryView<InputDevice> inputDeviceView(registry);
	for (const EntityId entityId : inputDeviceView)
	{
		InputDevice* inputDevice = registry.getComponent<InputDevice>(entityId);
		inputDevice->buttonBuffer.clear();
	}
}
