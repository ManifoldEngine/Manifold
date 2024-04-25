#pragma once

#include <string>
#include <unordered_map>
#include "InputAction.h"

namespace Mani
{
	struct InputUser
	{
		// user actions
		std::unordered_map<std::string, InputAction> actions;
		// control to actions binding
		std::unordered_map<std::string, std::vector<std::string>> bindings;
		
		std::vector<EntityId> inputDevices;
	};
}