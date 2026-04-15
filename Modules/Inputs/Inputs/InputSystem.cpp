#include "InputSystem.h"

#include <Core/Async/Parallel.h>
#include <Core/Debug/Profiling.h>

#include <Inputs/Inputs.h>
#include <Inputs/Cursor.h>

#include <Inputs/Components/InputUser.h>
#include <Inputs/Components/InputDevice.h>
#include <Inputs/Components/UniqueControlIdGenerator.h>

#include <ManiZ/ManiZ.h>

using namespace Mani;

void foreachBindings(const Map<ControlId, List<ActionId>>& bindings, List<InputAction>& actions, ControlId controlId, auto&& f)
{
	if (const List<ActionId>* actionBindings = bindings.find(controlId))
	{
		for (const ActionId actionId : *actionBindings)
		{
			MANI_ASSERT(actions.isValid(actionId), "Trying to access an invalid action id");
			InputAction& action = actions[actionId];
			f(action);
		}
	}
}

Vec3f toVector(EInputAxis axis)
{
	switch (axis)
	{
		case EInputAxis::Right : return VEC3F::RIGHT;
		case EInputAxis::Left: return VEC3F::LEFT;
		case EInputAxis::Up: return VEC3F::UP;
		case EInputAxis::Down: return VEC3F::DOWN;
		case EInputAxis::Forward: return VEC3F::FORWARD;
		case EInputAxis::Back: return VEC3F::BACK;
		default: return VEC3F::ZERO;
	}
}

void InputSystem::onInitialize(ECS::Registry& registry, World& world)
{
	registry.addSingle<Cursor>();
	registry.addSingle<UniqueControlIdGenerator>();
}

void InputSystem::onDeinitialize(ECS::Registry& registry, World& world)
{
	registry.removeSingle<UniqueControlIdGenerator>();
	registry.removeSingle<Cursor>();
}

void InputSystem::tick(ECS::Registry& registry)
{
	ECS::View<InputUser> view(registry);
	parallelFor(view, [&](ECS::EntityId entityId, InputUser& inputUser)
	{
		// reset action axis state 
		for (auto& action : inputUser.actions)
		{
			// axis are reset each tick.
			action.x = 0.f;
			action.y = 0.f;
			action.z = 0.f;

			action.wasPressed = action.isPressed;
		}

		for (const auto deviceId : inputUser.inputDevices)
		{
			// button buffers
			Ref<InputDevice> device = registry.get<InputDevice>(deviceId);
			for (const ButtonControl& control : device->buttonBuffer)
			{
				if (const List<AxisActionBinding>* bindings = inputUser.buttonToAxisBindings.find(control.id))
				{
					for (const auto& binding : *bindings)
					{
						MANI_ASSERT(inputUser.actions.isValid(binding.actionId), "Trying to access an invalid action id");
						InputAction& action = inputUser.actions[binding.actionId];
						if (control.isPressed)
						{
							const Vec3f delta = toVector(binding.axis);
							action.x += delta.x;
							action.y += delta.y;
							action.z += delta.z;
							MANI_LOG_VERBOSE(LogInputs, "Action {} axis changed to ({}, {}, {})", action.name, action.x, action.y, action.z);
						}
					}
				}

				foreachBindings(inputUser.bindings, inputUser.actions, control.id, [&control](InputAction& action)
				{
					if (action.isPressed != control.isPressed)
					{
						action.isPressed = control.isPressed;
						MANI_LOG_VERBOSE(LogInputs, "Action {} state changed to {}", action.name, action.isPressed);
					}
				});
			}

			// axis
			for (const AxisControl& control : device->axis)
			{
				foreachBindings(inputUser.bindings, inputUser.actions, control.id, [&control](InputAction& action)
				{
					action.x += control.x;
					action.y += control.y;
					action.z += control.z;
					MANI_LOG_VERBOSE(LogInputs, "Action {} axis changed to ({}, {}, {})", action.name, action.x, action.y, action.z);
				});
			}
		}
	});

	// clear button buffers
	for (auto [entityId, device] : ECS::View<InputDevice>(registry))
	{
		device.buttonBuffer.clear();
	}
}