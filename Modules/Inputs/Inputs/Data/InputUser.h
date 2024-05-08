#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include "InputAction.h"

namespace Mani
{
	struct InputUser
	{
		// user actions
		std::unordered_map<std::string, InputAction> actions;
		// control to actions binding
		std::unordered_map<std::string, std::unordered_set<std::string>> bindings;
		
		std::vector<ECS::EntityId> inputDevices;
	};

	namespace InputUtils
	{
		inline void setAction(InputUser& inputUser, const std::string& action)
		{
			inputUser.actions[action] = { action };
		}

		inline void unsetAction(InputUser& inputUser, const std::string& action)
		{
			inputUser.actions.erase(action);
		}

		inline void addBinding(InputUser& inputUser, const std::string& control, const std::string& action)
		{
			inputUser.bindings[control].insert(action);
		}

		inline void removeBinding(InputUser& inputUser, const std::string& control, const std::string& action)
		{
			inputUser.bindings[control].erase(action);
		}
	}
}