#pragma once

#include <string_view>
#include <Core/CoreFwd.h>
// forward
#include <Inputs/Components/InputUser.h>
#include <Inputs/Components/InputDevice.h>
#include <Inputs/Data/InputHints.h>

namespace Mani
{
	constexpr LogChannel LogInputs("Inputs");
	
	namespace InputsStatics
	{
		// tries to resolve the hint into a ControlId by looking through the assigned devices of this user
		ControlId resolveHint(const ECS::Registry& registry, ECS::EntityId entityId, EInputHints hint);

		// adds an action by name to an entity with an InputUser component
		void addAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action);
		// removes an action by name from an entity with an InputUser component
		void removeAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action);

		// binds a control to an action for an entity with an InputUser component
		void bindAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, ControlId controlId);
		// unbinds a control from an action for an entity with an InputUser component
		void unbindAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, ControlId controlId);
		// binds a control to an action for an entity with an InputUser component
		void bindAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, EInputHints hint);
		// unbinds a control from an action for an entity with an InputUser component
		void unbindAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, EInputHints hint);
		
		// binds a button to an action's axis value
		void bindActionAxis(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, EInputAxis axis, ControlId controlId);
		// unbinds a button from an action's axis value
		void unbindActionAxis(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, EInputAxis axis, ControlId controlId);

		// binds a button to an action's axis value
		void bindActionAxis(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, EInputAxis axis, EInputHints hint);
		// unbinds a button from an action's axis value
		void unbindActionAxis(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, EInputAxis axis, EInputHints hint);
		
		// returns the action object for an entity with an InputUser component
		InputAction& getAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action);
		const InputAction& getAction(const ECS::Registry& registry, ECS::EntityId entityId, const std::string& action);

		// adds an action and then binds the control to it for an entity with an InputUser component
		void addAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, ControlId controlId);
		// adds an action and then binds the control to it for an entity with an InputUser component
		void addAction(ECS::Registry& registry, ECS::EntityId entityId, const std::string& action, EInputHints hint);

		// assigns a device to an InputUser by entity id
		void assignDevice(ECS::Registry& registry, ECS::EntityId entityId, ECS::EntityId deviceId);
		// assigns a device to an InputUser by name, will look through all devices to find it
		void assignDevice(ECS::Registry& registry, ECS::EntityId entityId, const std::string& deviceName);
		// assigns all devices to an InputUser
		void assignAllDevices(ECS::Registry& registry, ECS::EntityId entityId);

		// unassigns a device to an InputUser by entity id
		void unassignDevice(ECS::Registry& registry, ECS::EntityId entityId, ECS::EntityId deviceId);
		// unassigns a device to an InputUser by name, will look through all devices to find it
		void unassignDevice(ECS::Registry& registry, ECS::EntityId entityId, const std::string& deviceName);
		// unassigns all devices that are assigned to an InputUser
		void unassignAllDevices(ECS::Registry& registry, ECS::EntityId entityId);

		// finds a device's id by name
		ECS::EntityId findDeviceByName(const ECS::Registry& registry, const std::string& name);

		// generate a unique control id, used by devices to declare new controls
		ControlId generateNextControlId(ECS::Registry& registry);
	}
}