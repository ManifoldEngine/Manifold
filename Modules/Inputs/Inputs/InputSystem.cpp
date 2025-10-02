#include "InputSystem.h"

#include <Inputs/Inputs.h>
#include <Inputs/Cursor.h>

#include <Inputs/Data/InputUser.h>
#include <Inputs/Data/InputDevice.h>

using namespace Mani;

void Mani::InputSystem::onInitialize(ECS::Registry& registry, World& world)
{
	registry.addSingle<Cursor>();
}

void Mani::InputSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	registry.removeSingle<Cursor>();
}

void InputSystem::tick(ECS::Registry& registry)
{
	ECS::View<InputUser> inputUserView(registry);
	for (const ECS::EntityId entityId : inputUserView)
	{
		InputUser* inputUser = registry.get<InputUser>(entityId);
		
		for (auto& [name, action] : inputUser->actions)
		{
			// axis are reset each tick.
			action.x = 0.f;
			action.y = 0.f;
			action.z = 0.f;

			action.wasPressed = action.isPressed;
		}

		// consume assigned input device
		for (const ECS::EntityId deviceId : inputUser->inputDevices)
		{
			InputDevice* inputDevice = registry.get<InputDevice>(deviceId);
			if (inputDevice == nullptr)
			{
				continue;
			}

			// buttons
			for (const ButtonControl& control : inputDevice->buttonBuffer)
			{
				List<std::string>* boundActionNames = inputUser->bindings.find(control.name);
				if (boundActionNames == nullptr)
				{
					continue;
				}

				for (const std::string& actionName : *boundActionNames)
				{
					InputAction& action = inputUser->actions[actionName];
					if (action.isPressed != control.isPressed)
					{
						MANI_LOG_VERBOSE(LogInputs, "Action {} state changed to {}", action.name, action.isPressed);
						action.isPressed = control.isPressed;
					}
				}
			}

			// axis
			for (const AxisControl& axis : inputDevice->axis)
			{
				List<std::string>* boundActionNames = inputUser->bindings.find(axis.name);
				if (boundActionNames == nullptr)
				{
					continue;
				}

				for (const std::string& actionName : *boundActionNames)
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
	ECS::View<InputDevice> inputDeviceView(registry);
	for (const ECS::EntityId entityId : inputDeviceView)
	{
		InputDevice* inputDevice = registry.get<InputDevice>(entityId);
		inputDevice->buttonBuffer.clear();
	}
}