#include "simpleTests.h"
#include <Inputs/InputSystem.h>
#include <Inputs/Interfaces/InputGenerator.h>
#include <Core/System/SystemContainer.h>

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	SimpleTests::SimpleTestsRunner::runTests();
}
#endif

using namespace Mani;

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

	virtual void onInputSystemTick(float deltaTime, EntityRegistry& registry) override 
	{
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

	void setLeftBumper(float x)
	{
		m_leftBumper.x = x;
	}

	void setRightBumper(float x)
	{
		m_rightBumper.x = x;
	}

	std::vector<ButtonControl> buffer;
private:
	ButtonControl m_aButton = { "AButton" };
	ButtonControl m_bButton = { "BButton" };

	AxisControl m_leftStick = { "LeftStick" };
	AxisControl m_rightStick = { "LeftStick" };
	AxisControl m_rightBumper = { "RightBumper" };
	AxisControl m_leftBumper = { "LeftBumper" };
};

std::vector<InputAction> actionTemplate = 
{
	{ "Jump" },
	{ "Dodge" },
	{ "Move" },
	{ "Shoot" },
};

std::unordered_map<std::string, std::vector<std::string>> inputBindingsTemplate =
{
	{ "LeftStick", { "Move" }},
	{ "RightStick", { "Move" }},
	{ "AButton", { "Jump" }},
	{ "BButton", { "Dodge" }},
	{ "RightBumper", { "Shoot" }},
};

ST_SECTION_BEGIN(Inputs, "Inputs")
{
	ST_TEST(CreateInputUser, "Should create an input user and assign an input generator to it.")
	{
		SystemContainer systemContainer;
		systemContainer.initialize();

		std::vector<InputAction> registeredActions;
		std::shared_ptr<InputSystem> inputSystem = systemContainer.initializeDependency<InputSystem>().lock();

		EventHandle handle = inputSystem->onActionEvent.subscribe([&registeredActions](uint32_t userId, const InputAction& action) 
		{
			registeredActions.push_back(action);
		});

		std::shared_ptr<VirtualController> controller = std::make_shared<VirtualController>();
		
		// press A button before pluging in the virtual controller
		controller->setAButton(true);

		systemContainer.tick(.0f);

		ST_ASSERT(registeredActions.size() == 0, "Should not have registered an input.");

		// create a user and assign them the virtual controller.
		const uint32_t userId = inputSystem->createInputUser(actionTemplate, inputBindingsTemplate);
		
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

		inputSystem->onActionEvent.unsubscribe(handle);
		systemContainer.deinitialize();
	}

	ST_TEST(MultiAxisInteraction, "Two opposite axis on the same direction should result in zero")
	{
		SystemContainer systemContainer;
		systemContainer.initialize();

		std::shared_ptr<InputSystem> inputSystem = systemContainer.initializeDependency<InputSystem>().lock();

		std::shared_ptr<VirtualController> controller = std::make_shared<VirtualController>();

		// create a user and assign them the virtual controller.
		std::vector<InputAction> actions =
		{
			InputAction { "Move" }
		};
		
		std::unordered_map<std::string, std::vector<std::string>> bindings =
		{
			{ "LeftStick", { "Move" }},
			{ "RightStick", { "Move" }}
		};

		const uint32_t userId = inputSystem->createInputUser(actions, bindings);

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

		const InputAction* moveAction = inputSystem->getAction(userId, "Move");
		ST_ASSERT(moveAction->x <= FLT_EPSILON, "Move X axis should be zero");

		systemContainer.deinitialize();
	}

	ST_TEST(PressUnassignedButton, "Should press an unassigned button without side effects")
	{
		SystemContainer systemContainer;
		systemContainer.initialize();

		bool bHasActionBeenTriggered = false;

		std::shared_ptr<InputSystem> inputSystem = systemContainer.initializeDependency<InputSystem>().lock();
		EventHandle handle = inputSystem->onActionEvent.subscribe([&bHasActionBeenTriggered](uint32_t userId, const InputAction& inputAction) 
		{
			bHasActionBeenTriggered = true;
		});

		std::shared_ptr<VirtualController> controller = std::make_shared<VirtualController>();

		// create a user and assign them the virtual controller.
		const uint32_t userId = inputSystem->createInputUser({}, {});

		// assign virtual controller to user
		inputSystem->assignInputGenerator(userId, controller);

		controller->setAButton(true);
		controller->setBButton(true);
		controller->setLeftStick(-1.f, 1.f);
		controller->setRightStick(1.f, -1.f);
		controller->setLeftBumper(1.f);
		controller->setRightBumper(1.f);

		// this should not crash.
		systemContainer.tick(0.f);
		
		ST_ASSERT(bHasActionBeenTriggered == false, "Action should not have been triggered.");

		inputSystem->onActionEvent.unsubscribe(handle);
		
		systemContainer.deinitialize();
	}

	ST_TEST(AssignControlToMultipleAction, "Should press receive multiple actions for a single control")
	{
		SystemContainer systemContainer;
		systemContainer.initialize();

		std::vector<InputAction> actionEvents;

		std::shared_ptr<InputSystem> inputSystem = systemContainer.initializeDependency<InputSystem>().lock();
		EventHandle handle = inputSystem->onActionEvent.subscribe([&actionEvents](uint32_t userId, const InputAction& inputAction)
			{
				actionEvents.push_back(inputAction);
			});

		std::shared_ptr<VirtualController> controller = std::make_shared<VirtualController>();

		// create a user and assign them the virtual controller.
		const uint32_t userId = inputSystem->createInputUser(
			{
				{ "Jump" },
				{ "Dodge" }
			}, 
			{
				{
					"AButton",
					{
						"Jump",
						"Dodge",
					}
				}
			});

		// assign virtual controller to user
		inputSystem->assignInputGenerator(userId, controller);

		controller->setAButton(true);
		
		// this should not crash.
		systemContainer.tick(0.f);

		ST_ASSERT(actionEvents.size() == 2, "Should have registered 2 actions");
		ST_ASSERT(actionEvents[0].name == "Jump", "First action should have been jump");
		ST_ASSERT(actionEvents[1].name == "Dodge", "Second action should have been jump");

		inputSystem->onActionEvent.unsubscribe(handle);

		systemContainer.deinitialize();
	}
}
ST_SECTION_END(Inputs)