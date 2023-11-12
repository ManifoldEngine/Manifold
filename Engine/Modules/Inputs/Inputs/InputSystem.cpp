#include "InputSystem.h"

#include <Log.h>
#include <Inputs/Interfaces/InputGenerator.h>

using namespace ECSEngine;

std::string_view InputSystem::getName() const
{
	return "InputSystem";
}

bool InputSystem::shouldTick(EntityRegistry& registry) const
{
	return true;
}

void InputSystem::tick(float deltaTime, EntityRegistry& registry)
{
	for (const auto& user : m_users)
	{
		if (user->generator == nullptr)
		{
			continue;
		}

		user->generator->onInputSystemTick(deltaTime, registry);

		std::vector<ButtonControl> buffer;
		if (user->generator->consumeInputBuffer(buffer))
		{
			for (const ButtonControl& buttonEvent : buffer)
			{
				const std::vector<std::string>& boundActionNames = user->bindings[buttonEvent.name];
				for (const std::string& actionName : boundActionNames)
				{
					InputAction& action = user->actions[actionName];
					if (action.isPressed != buttonEvent.isPressed)
					{
						action.isPressed = buttonEvent.isPressed;
						onActionEvent.broadcast(user->userId, action);
					}
				}
			}
		}
		else
		{
			ECSE_LOG_WARNING(LogInputs, "Could not get input buffer from generator {}", user->generator->getName());
		}
		
		std::vector<AxisControl> axis;
		if (user->generator->getAxis(axis))
		{
			// axis are reset each tick.
			for (auto& [name, action] : user->actions)
			{
				action.x = 0.f;
				action.y = 0.f;
				action.z = 0.f;
			}

			for (const AxisControl& axisControl : axis)
			{
				const std::vector<std::string>& boundActionNames = user->bindings[axisControl.name];
				for (const std::string& actionName : boundActionNames)
				{
					InputAction& action = user->actions[actionName];

					action.x += axisControl.x;
					action.y += axisControl.y;
					action.z += axisControl.z;
				}	
			}
		}
		else
		{
			ECSE_LOG_WARNING(LogInputs, "Could not get axis from generator {}", user->generator->getName());
		}
	}
}

const InputAction* InputSystem::getAction(uint32_t userId, const std::string& name)
{
	std::shared_ptr<InputUser> user = getInputUser(userId);
	if (user == nullptr)
	{
		return nullptr;
	}

	const InputAction& action = user->actions[name];
	if (action.name.empty())
	{
		return nullptr;
	}

	return &action;
}

uint32_t InputSystem::createInputUser(const std::vector<InputAction>& actions, const std::unordered_map<std::string, std::vector<std::string>>& bindings)
{
	std::shared_ptr<InputUser> user = std::make_shared<InputUser>();
	
	// iterate by copy
	for (const InputAction action : actions)
	{
		user->actions[action.name] = action;
	}

	user->bindings = bindings;

	user->userId = userCounter++;
	m_users.push_back(user);
	return 0;
}

std::shared_ptr<InputUser> InputSystem::getInputUser(uint32_t userId)
{
	const auto it = std::find_if(m_users.begin(), m_users.end(), [&userId](const std::shared_ptr<InputUser>& inputUser) {
		return userId == inputUser->userId;
	});

	if (it == m_users.end())
	{
		return nullptr;
	}
	
	return *it;
}

void InputSystem::destroyInputUser(uint32_t userId)
{
	const auto it = std::find_if(m_users.begin(), m_users.end(), [&userId](const std::shared_ptr<InputUser>& inputUser) {
		return userId == inputUser->userId;
	});

	if (it != m_users.end())
	{
		m_users.erase(it);
	}
}

void InputSystem::assignInputGenerator(uint32_t userId, std::shared_ptr<IInputGenerator> generator)
{
	std::shared_ptr<InputUser> inputUser = getInputUser(userId);
	if (inputUser == nullptr)
	{
		return;
	}

	inputUser->generator = generator;
}
