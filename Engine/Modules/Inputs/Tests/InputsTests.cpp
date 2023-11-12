#include "simpleTests.h"
#include <Inputs/InputSystem.h>
#include <Inputs/Interfaces/InputGenerator.h>
#include <Core/System/SystemContainer.h>

extern "C" __declspec(dllexport) void runTests()
{
	SimpleTests::SimpleTestsRunner::runTests();
}

using namespace ECSEngine;

class VirtualController : public IInputGenerator
{
public:
	virtual std::string getName() const override
	{
		return "My Cool Joystick";
	}

	virtual bool consumeInputBuffer(std::vector<ButtonControl>& outBuffer) override
	{
		outBuffer = buffer;
		buffer.clear();
		return true;
	}

	virtual bool getAxis(std::vector<AxisControl>& outAxis) override
	{
		outAxis.clear();
		outAxis.push_back(m_leftStick);
		outAxis.push_back(m_rightStick);
		outAxis.push_back(m_leftBumper);
		outAxis.push_back(m_rightBumper);
		return true;
	}

	void setLeftStick(float x, float y)
	{
		m_leftStick.x = x;
		m_leftStick.y = y;
	}

	void setRightStick(float x, float y)
	{
		m_rightStick.x = x;
		m_rightStick.y = y;
	}

	void setAButton(bool isPressed)
	{
		m_aButton.isPressed = isPressed;
		buffer.push_back(m_aButton);
	}

	void setBButton(bool isPressed)
	{
		m_bButton.isPressed = isPressed;
		buffer.push_back(m_bButton);
	}

	void setRightBumper(float x)
	{
		m_rightBumper.x = x;
	}

	std::vector<ButtonControl> buffer;
private:
	AxisControl m_leftStick = {
		"LeftStick",
		0.f,
		0.f,
		0.f,
	};

	AxisControl m_rightStick = {
		"LeftStick",
		0.f,
		0.f,
		0.f,
	};

	ButtonControl m_aButton = {
		"AButton",
		false
	};

	ButtonControl m_bButton = {
		"BButton",
		false
	};

	AxisControl m_rightBumper = {
		"RightBumper",
		0.f,
		0.f,
		0.f,
	};

	AxisControl m_leftBumper = {
		"LeftBumper",
		0.f,
		0.f,
		0.f,
	};
};

std::unordered_map<std::string, InputAction> actionTemplate = {
	{
		"Jump",
			InputAction
		{
			"Jump",
			0.f,
			0.f,
			0.f,
			false,
			true,
		},
	},
	{
		"Dodge",
		InputAction
		{
			"Dodge",
			0.f,
			0.f,
			0.f,
			false,
			true,
		},
	},
	{
		"Move",
		InputAction
		{
			"Move",
			0.f,
			0.f,
			0.f,
			false,
			true,
		},
	},
	{
		"Shoot",
		InputAction
		{
			"Shoot",
			0.f,
			0.f,
			0.f,
			false,
			true,
		},
	},
};

std::unordered_map<std::string, std::string> inputBindingsTemplate =
{
	{ "LeftStick", "Move" },
	{ "RightStick", "Move" },
	{ "AButton", "Jump" },
	{ "BButton", "Dodge" },
	{ "RightBumper", "Shoot" },
};

ST_SECTION_BEGIN(Inputs, "Inputs")
{
	ST_TEST(CreateInputUser, "Should create an input user and assign an input generator to it.")
	{
		SystemContainer systemContainer;
		systemContainer.initialize();

		std::vector<InputAction> registeredActions;
		std::shared_ptr<InputSystem> inputSystem = systemContainer.initializeDependency<InputSystem>().lock();

		EventHandle handle = inputSystem->onActionChanged.subscribe([&registeredActions](uint32_t userId, const InputAction& action) {
			registeredActions.push_back(action);
		});

		std::shared_ptr<VirtualController> controller = std::make_shared<VirtualController>();
		
		// press A button before pluging in the virtual controller
		controller->setAButton(true);

		systemContainer.tick(.0f);

		ST_ASSERT(registeredActions.size() == 0, "Should not have registered an input.");

		// create a user and assign them the virtual controller.
		const uint32_t userId = inputSystem->createInputUser();
		std::shared_ptr<InputUser> user = inputSystem->getInputUser(userId);
		user->actions = actionTemplate;
		user->inputBindings = inputBindingsTemplate;

		// assign virtual controller to user
		inputSystem->assignInputGenerator(userId, controller);

		// tick the system, consumes input buffers
		systemContainer.tick(.0f);

		ST_ASSERT(registeredActions.size() == 1, "Should have registered an input because the button was already pressed.");

		// left stick diagonal right+up
		controller->setLeftStick(1.f, 1.f);
		// release A button
		controller->setAButton(false);
		// then press B button
		controller->setBButton(true);

		// tick the system, consumes input buffers
		systemContainer.tick(.0f);
		
		ST_ASSERT(registeredActions.size() == 3, "Should have registered an input");
		
		InputAction secondToLastAction = registeredActions[registeredActions.size() - 2];
		ST_ASSERT(secondToLastAction.name == "Jump" && secondToLastAction.isPressed == false, "Last action should be Jump false");

		InputAction lastAction = registeredActions.back();
		ST_ASSERT(lastAction.name == "Dodge" && lastAction.isPressed == true, "Last action should be Dodge true");

		inputSystem->onActionChanged.unsubscribe(handle);
		systemContainer.deinitialize();
	}

	ST_TEST(MultiAxisInteraction, "Two opposite axis on the same direction should result in zero")
	{
		SystemContainer systemContainer;
		systemContainer.initialize();

		std::vector<InputAction> registeredActions;
		std::shared_ptr<InputSystem> inputSystem = systemContainer.initializeDependency<InputSystem>().lock();

		std::shared_ptr<VirtualController> controller = std::make_shared<VirtualController>();

		// create a user and assign them the virtual controller.
		const uint32_t userId = inputSystem->createInputUser();
		std::shared_ptr<InputUser> user = inputSystem->getInputUser(userId);
		user->actions =
		{
			{
				"Move",
				InputAction
				{
					"Move",
					0.f,
					0.f,
					0.f,
					false,
					true,
				}
			}
		};

		user->inputBindings =
		{
			{ "LeftStick", "Move" },
			{ "RightStick", "Move" }
		};

		// assign virtual controller to user
		inputSystem->assignInputGenerator(userId, controller);

		// tick the system, consumes input buffers
		systemContainer.tick(.0f);

		// left stick full left
		controller->setLeftStick(-1.f, 0.f);
		// right stick full right
		controller->setRightStick(1.f, 0.f);

		// tick the system, consumes input buffers
		systemContainer.tick(.0f);

		ST_ASSERT(user->actions["Move"].x <= FLT_EPSILON, "Move X axis should be zero");

		systemContainer.deinitialize();
	}
}
ST_SECTION_END(Inputs)