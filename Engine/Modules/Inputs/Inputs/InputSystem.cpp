#include "InputSystem.h"

#include <Log.h>
#include <Inputs/Interfaces/InputGenerator.h>

using namespace Mani;

std::string_view InputSystem::getName() const
{
	return "InputSystem";
}

bool InputSystem::shouldTick(EntityRegistry& registry) const
{
	return true;
}

void InputSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	createInputUser({}, {});
}

void InputSystem::tick(float deltaTime, EntityRegistry& registry)
{
	for (const auto& user : m_users)
	{
		std::weak_ptr weakGenerator = m_generators[user->userId];
		if (weakGenerator.expired())
		{
			continue;
		}

		std::shared_ptr<IInputGenerator> generator = weakGenerator.lock();

		generator->onInputSystemTick(deltaTime, registry);

		std::vector<ButtonControl> buffer;
		if (generator->consumeInputBuffer(buffer))
		{
			for (const ButtonControl& buttonEvent : buffer)
			{
				const std::vector<std::string>& boundActionNames = user->bindings[buttonEvent.name];
				for (const std::string& actionName : boundActionNames)
				{
					InputAction& action = user->actions[actionName];
					if (action.isPressed != buttonEvent.isPressed)
					{
						MANI_LOG_VERBOSE(LogInputs, "Action {} state changed to {}", action.name, action.isPressed);
						action.isPressed = buttonEvent.isPressed;
						onActionEvent.broadcast(user->userId, action);
					}
				}
			}
		}
		else
		{
			MANI_LOG_WARNING(LogInputs, "Could not get input buffer from generator {}", generator->getName());
		}
		
		std::vector<AxisControl> axis;
		if (generator->getAxis(axis))
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

#if MANI_DEBUG
			for (auto& [name, action] : user->actions)
			{
				// log action state
				MANI_LOG_VERBOSE(LogInputs, "Action {} axis changed to ({}, {}, {})", action.name, action.x, action.y, action.z);
			}
#endif
		}
		else
		{
			MANI_LOG_WARNING(LogInputs, "Could not get axis from generator {}", generator->getName());
		}
	}
}

const InputAction* InputSystem::getAction(uint32_t userId, const std::string& name) const
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
	return user->userId;
}

std::shared_ptr<InputUser> InputSystem::getInputUser(uint32_t userId) const
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

void InputSystem::assignInputGenerator(uint32_t userId, const std::weak_ptr<IInputGenerator>& generator)
{
	m_generators[userId] = generator;
}

bool InputSystem::addAction(uint32_t userId, const std::string& name) 
{
	std::shared_ptr<InputUser> user = getInputUser(userId);
	if (user == nullptr)
	{
		return false;
	}

	user->actions[name] = InputAction{ name };
	return true;
}

bool InputSystem::removeAction(uint32_t userId, const std::string& name) 
{
	std::shared_ptr<InputUser> user = getInputUser(userId);
	if (user == nullptr)
	{
		return false;
	}

	return user->actions.erase(name) > 0;
}

bool InputSystem::addBinding(uint32_t userId, const std::string& action, const std::string control)
{
	std::shared_ptr<InputUser> user = getInputUser(userId);
	if (user == nullptr)
	{
		return false;
	}

	std::vector<std::string>& actions = user->bindings[control];
	actions.push_back(action);
	return true;
}

bool InputSystem::removeBinding(uint32_t userId, const std::string& action, const std::string control) 
{
	std::shared_ptr<InputUser> user = getInputUser(userId);
	if (user == nullptr)
	{
		return false;
	}

	std::vector<std::string>& actions = user->bindings[control];
	auto it = std::find(actions.begin(), actions.end(), action);
	
	if (it != actions.end())
	{
		actions.erase(it);
		return true;
	}
	
	return false;
}