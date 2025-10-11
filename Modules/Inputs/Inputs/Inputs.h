#pragma once

#include <string_view>
#include <Core/CoreFwd.h>
// forward
#include <Inputs/Data/InputUser.h>
#include <Inputs/Data/InputDevice.h>

namespace Mani
{
    const std::string_view LogInputs = "Inputs";
	
	namespace InputsStatics
	{
		// adds an action by name to a entity with an InputUser component
		void addAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action);
		// removes an action by name to a entity with an InputUser component
		void removeAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action);

		// binds a control to an action for an entity with an InputUser component
		void bindAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, const std::string& control);
		// unbinds a control to an action for an entity with an InputUser component
		void unbindAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, const std::string& control);
		
		// returns the action object for an entity witn an InputUser component
		Mani::InputAction& getAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action);
		const Mani::InputAction& getAction(const ECS::Registry& registry, ECS::EntityId entityId, const std::string& action);

		// adds an action and then binds the control to it for an entity with an InputUser component
		void addAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, const std::string& control);

		// assigns a device to an InputUser by entity id
		void assignDevice(ECS::Registry& registry, ECS::EntityId entityId, ECS::EntityId deviceId);
		// assigns a device to an InputUser by name, will look through all devices to find it
		void assignDevice(ECS::Registry& registry, ECS::EntityId entityId, const std::string& deviceName);

		// unassigns a device to an InputUser by entity id
		void unassignDevice(ECS::Registry& registry, ECS::EntityId entityId, ECS::EntityId deviceId);
		// unassigns a device to an InputUser by name, will look through all devices to find it
		void unassignDevice(ECS::Registry& registry, ECS::EntityId entityId, const std::string& deviceName);

		// finds a device's id by name
		ECS::EntityId findDeviceByName(const ECS::Registry& registry, const std::string& name);
	}
}