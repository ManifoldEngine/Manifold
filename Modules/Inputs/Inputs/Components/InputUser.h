#pragma once

#include <Core/Containers/Map.h>
#include <Core/Containers/List.h>
#include <Inputs/Data/InputAction.h>
#include <Inputs/Data/InputControl.h>

#include <string>

namespace Mani
{
	enum class EInputAxis : uint8_t
	{
		Right = 0,
		Left,
		Up,
		Down,
		Forward,
		Back,
	};

	struct AxisActionBinding
	{
		EInputAxis axis = EInputAxis::Right;
		ActionId actionId = INVALID_INPUT_ACTION_ID;
	};

	struct PendingBindingRequest
	{
		EInputHints hint = EInputHints::None;
		std::string action = "";
	};

	struct PendingAxisBindingRequest
	{
		EInputHints hint = EInputHints::None;
		EInputAxis axis = EInputAxis::Right;
		std::string action = "";
	};

	struct InputUser
	{
		// user actions
		List<InputAction> actions;

		// control to actions binding
		Map<ControlId, List<ActionId>> bindings;

		// allows to bind buttons to action's axis
		Map<ControlId, List<AxisActionBinding>> buttonToAxisBindings;

		// pending binding by hint
		List<PendingBindingRequest> pendingBindingRequests;

		// pending binding by hint
		List<PendingAxisBindingRequest> pendingButtonToAxisBindings;

		List<EntityId> inputDevices;
	};
}