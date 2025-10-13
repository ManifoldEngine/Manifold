#include "Inputs.h"
#include <Inputs/Components/InputUser.h>
#include <Inputs/Components/InputDevice.h>
#include <Inputs/Components/UniqueControlIdGenerator.h>

using namespace Mani;

ActionId resolveActionName(const InputUser& user, const std::string& actionName)
{
	const SizeT index = user.actions.indexOfIf([&actionName] (const InputAction& action)
	{
		return action.name == actionName;
	});
	return static_cast<ActionId>(index);
}

ActionId resolveActionNameChecked(const InputUser& user, const std::string& actionName)
{
	ActionId actionId = resolveActionName(user, actionName);
	MANI_ASSERT(actionId != INVALID_INPUT_ACTION_ID, "{} resolved in an invalid id", actionName);
	return actionId;
}

void resolvePendingBindingRequests(ECS::Registry& registry, ECS::EntityId entityId)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	List<PendingBindingRequest>& requests = inputUser.pendingBindingRequests;
	for (int index = static_cast<int>(requests.count() - 1); index >= 0; --index)
	{
		PendingBindingRequest& request = requests[index];
		const ControlId controlId = InputsStatics::resolveHint(registry, entityId, request.hint);
		if (controlId == INVALID_CONTROL_ID)
		{
			continue;
		}
		
		InputsStatics::bindAction(registry, entityId, request.action, controlId);
		requests.removeAt(index);
	}
}

void resolvePendingButtonToAxisBindings(ECS::Registry& registry, ECS::EntityId entityId)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	List<PendingAxisBindingRequest>& requests = inputUser.pendingButtonToAxisBindings;
	for (int index = static_cast<int>(requests.count() - 1); index >= 0; --index)
	{
		PendingAxisBindingRequest& request = requests[index];
		const ControlId controlId = InputsStatics::resolveHint(registry, entityId, request.hint);
		if (controlId == INVALID_CONTROL_ID)
		{
			continue;
		}

		InputsStatics::bindActionAxis(registry, entityId, request.action, request.axis, controlId);
		requests.removeAt(index);
	}
}

ControlId InputsStatics::resolveHint(const ECS::Registry& registry, ECS::EntityId entityId, EInputHints hint)
{
	ControlId result = INVALID_CONTROL_ID;
	const InputUser& inputUser = registry.getRef<InputUser>(entityId);
	for (const auto deviceId : inputUser.inputDevices)
	{
		const InputDevice& device = registry.getRef<InputDevice>(deviceId);
		if (const ControlId* controlId = device.buttonHints.find(hint))
		{
			result = *controlId;
			break;
		}

		for (const AxisControl& axis : device.axis)
		{
			if (axis.hint == hint)
			{
				result = axis.id;
				break;
			}
		}
	}

	return result;
}

void InputsStatics::addAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& actionName)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	MANI_ASSERT(resolveActionName(inputUser, actionName) == INVALID_INPUT_ACTION_ID, "There's already an action with this name");
	
	const InputAction action = {
		.id = inputUser.actions.count(),
		.name = actionName,
	};
	inputUser.actions.add(action);
}

void InputsStatics::removeAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	const ActionId actionId = resolveActionName(inputUser, action);
	MANI_ASSERT(actionId != INVALID_INPUT_ACTION_ID, "Trying to remove non existant action");
	inputUser.actions.removeAt(actionId);
}

void InputsStatics::bindAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, ControlId controlId)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	inputUser.bindings[controlId].addUnique(resolveActionNameChecked(inputUser, action));
}

void InputsStatics::unbindAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, ControlId controlId)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	inputUser.bindings[controlId].remove(resolveActionNameChecked(inputUser, action));
}

void InputsStatics::bindAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, EInputHints hint)
{
	const ControlId controlId = resolveHint(registry, entityId, hint);
	if (controlId == INVALID_CONTROL_ID)
	{
		InputUser& inputUser = registry.getRef<InputUser>(entityId);
		inputUser.pendingBindingRequests.add({ hint, action });
		return;
	}

	bindAction(registry, entityId, action, controlId);
}

void InputsStatics::unbindAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, EInputHints hint)
{
	const ControlId controlId = resolveHint(registry, entityId, hint);
	if (controlId == INVALID_CONTROL_ID)
	{
		InputUser& inputUser = registry.getRef<InputUser>(entityId);
		inputUser.pendingBindingRequests.removeIf([&](const PendingBindingRequest& request) { return request.hint == hint && request.action == action; });
		return;
	}

	unbindAction(registry, entityId, action, controlId);
}

void InputsStatics::bindActionAxis(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, EInputAxis axis, ControlId controlId)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	const ActionId actionId = resolveActionNameChecked(inputUser, action);
	
	List<AxisActionBinding>& bindings = inputUser.buttonToAxisBindings[controlId];

	auto ptr = bindings.findIf([&](const AxisActionBinding& binding) { return binding.axis == axis && binding.actionId == actionId; });
	MANI_ASSERT(ptr == nullptr, "A binding for this axis and action already exists");
	bindings.add({ axis, actionId });
}

void InputsStatics::unbindActionAxis(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, EInputAxis axis, ControlId controlId)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	const ActionId actionId = resolveActionNameChecked(inputUser, action);
	inputUser.buttonToAxisBindings[controlId].removeIf([&](const AxisActionBinding& binding) { return binding.axis == axis && binding.actionId == actionId; });
}

void InputsStatics::bindActionAxis(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, EInputAxis axis, EInputHints hint)
{
	const ControlId controlId = resolveHint(registry, entityId, hint);
	if (controlId == INVALID_CONTROL_ID)
	{
		InputUser& inputUser = registry.getRef<InputUser>(entityId);
		inputUser.pendingButtonToAxisBindings.add({ hint, axis, action });
		return;
	}

	bindActionAxis(registry, entityId, action, axis, controlId);
}

void InputsStatics::unbindActionAxis(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, EInputAxis axis, EInputHints hint)
{
	const ControlId controlId = resolveHint(registry, entityId, hint);
	if (controlId == INVALID_CONTROL_ID)
	{
		InputUser& inputUser = registry.getRef<InputUser>(entityId);
		inputUser.pendingButtonToAxisBindings.removeIf([&](const PendingAxisBindingRequest& request) { return request.hint == hint && request.axis == axis && request.action == action; });
		return;
	}

	unbindActionAxis(registry, entityId, action, axis, controlId);
}

InputAction& InputsStatics::getAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	return inputUser.actions[resolveActionNameChecked(inputUser, action)];
}

const InputAction& InputsStatics::getAction(const ECS::Registry& registry, ECS::EntityId entityId, const std::string& action)
{
	const InputUser& inputUser = registry.getRef<InputUser>(entityId);
	return inputUser.actions[resolveActionNameChecked(inputUser, action)];
}

void InputsStatics::addAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, ControlId controlId)
{
	addAction(registry, entityId, action);
	bindAction(registry, entityId, action, controlId);
}

void InputsStatics::addAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, EInputHints hint)
{
	const ControlId controlId = resolveHint(registry, entityId, hint);
	if (controlId == INVALID_CONTROL_ID)
	{
		addAction(registry, entityId, action);
		bindAction(registry, entityId, action, hint);
	}
	else
	{
		addAction(registry, entityId, action, controlId);
	}
}

void InputsStatics::assignDevice(ECS::Registry& registry, ECS::EntityId entityId, ECS::EntityId deviceId)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	inputUser.inputDevices.add(deviceId);
	resolvePendingBindingRequests(registry, entityId);
	resolvePendingButtonToAxisBindings(registry, entityId);
}

void InputsStatics::assignDevice(ECS::Registry& registry, ECS::EntityId entityId, const std::string& deviceName)
{
	const ECS::EntityId deviceId = findDeviceByName(registry, deviceName);
	MANI_ASSERT(registry.isValid(deviceId), "Could not find a valid device with name {}", deviceName);
	assignDevice(registry, entityId, deviceId);
}

void InputsStatics::unassignDevice(ECS::Registry& registry, ECS::EntityId entityId, ECS::EntityId deviceId)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	inputUser.inputDevices.remove(deviceId);
}

void InputsStatics::unassignDevice(ECS::Registry& registry, ECS::EntityId entityId, const std::string& deviceName)
{
	const ECS::EntityId deviceId = findDeviceByName(registry, deviceName);
	MANI_ASSERT(registry.isValid(deviceId), "Could not find a valid device with name {}", deviceName);
	unassignDevice(registry, entityId, deviceId);
}

ECS::EntityId InputsStatics::findDeviceByName(const ECS::Registry& registry, const std::string& name)
{
	for (const auto entityId : ECS::View<InputDevice>(registry))
	{
		if (registry.getRef<InputDevice>(entityId).deviceName == name)
		{
			return entityId;
		}
	}
	return ECS::INVALID_ID;
}

ControlId InputsStatics::generateNextControlId(ECS::Registry& registry)
{
	UniqueControlIdGenerator* generator = registry.getSingle<UniqueControlIdGenerator>();
	MANI_ASSERT(generator != nullptr, "Trying to generact a controlid without the InputSystem being initialized");
	return generator->generateNextId();
}
