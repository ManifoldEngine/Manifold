#include "Inputs.h"
#include <Inputs/Data/InputUser.h>
#include <Inputs/Data/InputDevice.h>

using namespace Mani;

void InputsStatics::addAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	inputUser.actions[action] = { action };
}

void InputsStatics::removeAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	inputUser.actions.remove(action);
}

void InputsStatics::bindAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, const std::string& control)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	inputUser.bindings[control].addUnique(action);
}

void InputsStatics::unbindAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, const std::string& control)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	inputUser.bindings[control].remove(action);
}

InputAction& InputsStatics::getAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	return inputUser.actions.get(action);
}

const InputAction& InputsStatics::getAction(const ECS::Registry& registry, ECS::EntityId entityId, const std::string& action)
{
	const InputUser& inputUser = registry.getRef<InputUser>(entityId);
	return inputUser.actions.get(action);
}

void InputsStatics::addAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, const std::string& control)
{
	addAction(registry, entityId, action);
	bindAction(registry, entityId, action, control);
}

void InputsStatics::assignDevice(ECS::Registry& registry, ECS::EntityId entityId, ECS::EntityId deviceId)
{
	InputUser& inputUser = registry.getRef<InputUser>(entityId);
	inputUser.inputDevices.add(deviceId);
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
