#include <ManiTests/ManiTests.h>
#include <Core/ManiTests.h>

#include <Inputs/Inputs.h>
#include <Inputs/InputSystem.h>

#include <Core/World.h>

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	Mani::runTests();
}
#endif

using namespace Mani;

class VirtualController
{
public:
	VirtualController(ECS::Registry& registry) : 
        m_registry(&registry)
	{
		m_deviceId = m_registry->create();
		Ref<InputDevice> device = m_registry->add<InputDevice>(m_deviceId);
		device->deviceName = "Virtual Controller";
		device->axis.add(AxisControl{
			.id = leftJoystickId,
            .hint = EInputHints::Gamepad_Left_Stick,
		});

		aButtonId = Inputs::generateNextControlId(registry);
		bButtonId = Inputs::generateNextControlId(registry);

		dpadLeftId = Inputs::generateNextControlId(registry);
		dpadUpId = Inputs::generateNextControlId(registry);
		dpadDownId = Inputs::generateNextControlId(registry);
		dpadRightId = Inputs::generateNextControlId(registry);
		
		leftJoystickId = Inputs::generateNextControlId(registry);

        device->buttonHints = {
            { EInputHints::Gamepad_Bottom_FaceButton, aButtonId },
            { EInputHints::Gamepad_Right_FaceButton, bButtonId },

            { EInputHints::Gamepad_DPad_Left, dpadLeftId },
            { EInputHints::Gamepad_DPad_Up, dpadUpId },
            { EInputHints::Gamepad_DPad_Right, dpadRightId },
            { EInputHints::Gamepad_DPad_Down, dpadDownId },

            { EInputHints::Gamepad_Left_Stick_Button, leftJoystickId },
        };
	}

	~VirtualController()
	{
		m_registry->destroy(m_deviceId);
		m_registry = nullptr;
	}

	void setLeftJoystick(const Mani::Vec2f& pos)
	{
		Ref<InputDevice> device = m_registry->get<InputDevice>(m_deviceId);
		const Mani::Vec2f& p = pos.normalize();
		device->axis[0].x = p.x;
		device->axis[0].y = p.y;
	}

	void press(ControlId buttonId)
	{
		Ref<InputDevice> device = m_registry->get<InputDevice>(m_deviceId);
		device->buttonBuffer.add(ButtonControl{ .id = buttonId, .isPressed = true });
	}

	void release(ControlId buttonId)
	{
		Ref<InputDevice> device = m_registry->get<InputDevice>(m_deviceId);
		device->buttonBuffer.add(ButtonControl{ .id = buttonId, .isPressed = false });
	}

	ECS::EntityId getDeviceId() const { return m_deviceId; }

	ControlId aButtonId = INVALID_CONTROL_ID;
	ControlId bButtonId = INVALID_CONTROL_ID;

	ControlId dpadLeftId = INVALID_CONTROL_ID;
	ControlId dpadUpId = INVALID_CONTROL_ID;
	ControlId dpadDownId = INVALID_CONTROL_ID;
	ControlId dpadRightId = INVALID_CONTROL_ID;

	ControlId leftJoystickId = INVALID_CONTROL_ID;

private:
	Mani::List<ControlId> m_buffer;

	bool m_aButton = false;
	bool m_bButton = false;

	bool m_dpadUp = false;
	bool m_dpadDown = false;
	bool m_dpadRight = false;
	bool m_dpadLeft = false;

	Mani::Vec2f m_leftJoystick = Mani::VEC2F::ZERO;

	ECS::Registry* m_registry = nullptr;
	ECS::EntityId m_deviceId = ECS::INVALID_ID;
};

MANI_SECTION_BEGIN(Inputs, "Inputs")
{
	MANI_TEST(PressButtonA, "Should create an input user and assign an input generator to it, then press button a")
	{
		Application app;
		World& world = app.getWorld();
		world.initialize();
		world.createSystem<InputSystem>();

		ECS::Registry& registry = world.getMutableRegistry();
		VirtualController controller(registry);

		const ECS::EntityId userId = registry.create();
		registry.add<InputUser>(userId);
		Inputs::assignDevice(registry, userId, controller.getDeviceId());
		Inputs::addAction(registry, userId, "punch", controller.aButtonId);

		world.tick();

		{
			const InputAction& action = Inputs::getAction(registry, userId, "punch");
			MANI_TEST_ASSERT(!action.released() && !action.pressed() && !action.held() && !action.changed(), "action should not have been pressed");
		}

		controller.press(controller.aButtonId);
		world.tick();

		{
			const InputAction& action = Inputs::getAction(registry, userId, "punch");
			MANI_TEST_ASSERT(action.pressed(), "action should have been pressed");
		}

		world.tick();

		{
			const InputAction& action = Inputs::getAction(registry, userId, "punch");
			MANI_TEST_ASSERT(action.held(), "action should be held");
		}

		controller.release(controller.aButtonId);
		world.tick();

		{
			const InputAction& action = Inputs::getAction(registry, userId, "punch");
			MANI_TEST_ASSERT(action.released(), "action should have been released");
		}

		world.tick();

		{
			const InputAction& action = Inputs::getAction(registry, userId, "punch");
			MANI_TEST_ASSERT(!action.released() && !action.pressed() && !action.held() && !action.changed(), "action should not have been released");
		}
	}

    MANI_TEST(AddRemoveAction, "Should add and remove actions cleanly")
    {
        Application app;
        World& world = app.getWorld();
        world.initialize();

        ECS::Registry& registry = world.getMutableRegistry();
        const ECS::EntityId userId = registry.create();
        registry.add<InputUser>(userId);

        Inputs::addAction(registry, userId, "jump");
        Ref<InputUser> user = registry.get<InputUser>(userId);
        
        MANI_TEST_ASSERT(user->actions.findIf([](const InputAction& action) { return action.name == "jump"; }), "action added");

        Inputs::removeAction(registry, userId, "jump");
        Ref<InputUser> user2 = registry.get<InputUser>(userId);
        MANI_TEST_ASSERT(!user2->actions.findIf([](const InputAction& action) { return action.name == "jump"; }), "action removed");
    }

    MANI_TEST(BindUnbindAction, "Should bind and unbind a control to an action")
    {
        Application app;
        World& world = app.getWorld();
        world.initialize();
        world.createSystem<InputSystem>();

        ECS::Registry& registry = world.getMutableRegistry();
        VirtualController controller(registry);

        const ECS::EntityId userId = registry.create();
        registry.add<InputUser>(userId);
        Inputs::assignDevice(registry, userId, controller.getDeviceId());
        Inputs::addAction(registry, userId, "dash");

        Inputs::bindAction(registry, userId, "dash", controller.bButtonId);

        controller.press(controller.bButtonId);
        world.tick();
        MANI_TEST_ASSERT(Inputs::getAction(registry, userId, "dash").pressed(), "responds when bound");

        Inputs::unbindAction(registry, userId, "dash", controller.bButtonId);
        controller.release(controller.bButtonId);
        world.tick();
        controller.press(controller.bButtonId);
        world.tick();
        MANI_TEST_ASSERT(!Inputs::getAction(registry, userId, "dash").pressed(), "doesn't respond after unbind");
    }

    MANI_TEST(BindActionAxis, "Should bind dpad to axis values")
    {
        Application app;
        World& world = app.getWorld();
        world.initialize();
        world.createSystem<InputSystem>();

        ECS::Registry& registry = world.getMutableRegistry();
        VirtualController controller(registry);

        const ECS::EntityId userId = registry.create();
        registry.add<InputUser>(userId);
        Inputs::assignDevice(registry, userId, controller.getDeviceId());
        Inputs::addAction(registry, userId, "move");

        Inputs::bindActionAxis(registry, userId, "move", EInputAxis::Up, controller.dpadUpId);
        Inputs::bindActionAxis(registry, userId, "move", EInputAxis::Down, controller.dpadDownId);

        controller.press(controller.dpadUpId);
        world.tick();
        MANI_TEST_ASSERT(Inputs::getAction(registry, userId, "move").y > 0.0f, "up positive");

        controller.release(controller.dpadUpId);
        controller.press(controller.dpadDownId);
        world.tick();
        MANI_TEST_ASSERT(Inputs::getAction(registry, userId, "move").y < 0.0f, "down negative");
    }

    MANI_TEST(AssignDeviceByName, "Should assign and unassign a device by name")
    {
        Application app;
        World& world = app.getWorld();
        world.initialize();
        world.createSystem<InputSystem>();

        ECS::Registry& registry = world.getMutableRegistry();
        VirtualController controller(registry);
        const ECS::EntityId userId = registry.create();
        registry.add<InputUser>(userId);

        Inputs::assignDevice(registry, userId, "Virtual Controller");
        Ref<InputUser> user = registry.get<InputUser>(userId);
        MANI_TEST_ASSERT(!user->inputDevices.isEmpty(), "assigned by name");

        Inputs::unassignDevice(registry, userId, "Virtual Controller");
        Ref<InputUser> user2 = registry.get<InputUser>(userId);
        MANI_TEST_ASSERT(user2->inputDevices.isEmpty(), "unassigned by name");
    }

	MANI_TEST(FindDeviceByName, "Should locate a device by name")
    {
        Application app;
        World& world = app.getWorld();
        world.initialize();
        world.createSystem<InputSystem>();

        ECS::Registry& registry = world.getMutableRegistry();
        VirtualController controller(registry);

        ECS::EntityId found = Inputs::findDeviceByName(registry, "Virtual Controller");
        MANI_TEST_ASSERT(found == controller.getDeviceId(), "found correct device");
    }

	MANI_TEST(GenerateControlIdUniqueness, "Should generate unique control ids")
	{
        Application app;
        World& world = app.getWorld();
        world.initialize();
        world.createSystem<InputSystem>();

        ECS::Registry& registry = world.getMutableRegistry();
        ControlId a = Inputs::generateNextControlId(registry);
        ControlId b = Inputs::generateNextControlId(registry);
        ControlId c = Inputs::generateNextControlId(registry);

        MANI_TEST_ASSERT(a != b && b != c && a != c, "unique ids");
	}

    MANI_TEST(SimultaneousButtonPress, "Should correctly handle two actions pressed simultaneously")
    {
        Application app;
        World& world = app.getWorld();
        world.initialize();
        world.createSystem<InputSystem>();

        ECS::Registry& registry = world.getMutableRegistry();
        VirtualController controller(registry);

        const ECS::EntityId userId = registry.create();
        registry.add<InputUser>(userId);

        // Assign device
        Inputs::assignDevice(registry, userId, controller.getDeviceId());

        // Add two actions bound to two different buttons
        Inputs::addAction(registry, userId, "attack", controller.aButtonId);
        Inputs::addAction(registry, userId, "jump", controller.bButtonId);

        // Initial tick — both idle
        world.tick();
        {
            const InputAction& attack = Inputs::getAction(registry, userId, "attack");
            const InputAction& jump = Inputs::getAction(registry, userId, "jump");

            MANI_TEST_ASSERT(!attack.pressed() && !jump.pressed(), "both actions initially idle");
        }

        // Press both A and B at the same time
        controller.press(controller.aButtonId);
        controller.press(controller.bButtonId);
        world.tick();

        {
            const InputAction& attack = Inputs::getAction(registry, userId, "attack");
            const InputAction& jump = Inputs::getAction(registry, userId, "jump");

            MANI_TEST_ASSERT(attack.pressed(), "attack action pressed");
            MANI_TEST_ASSERT(jump.pressed(), "jump action pressed");
        }

        // Next frame: both should be held
        world.tick();
        {
            const InputAction& attack = Inputs::getAction(registry, userId, "attack");
            const InputAction& jump = Inputs::getAction(registry, userId, "jump");

            MANI_TEST_ASSERT(attack.held(), "attack held");
            MANI_TEST_ASSERT(jump.held(), "jump held");
        }

        // Release both
        controller.release(controller.aButtonId);
        controller.release(controller.bButtonId);
        world.tick();
        {
            const InputAction& attack = Inputs::getAction(registry, userId, "attack");
            const InputAction& jump = Inputs::getAction(registry, userId, "jump");

            MANI_TEST_ASSERT(attack.released(), "attack released");
            MANI_TEST_ASSERT(jump.released(), "jump released");
        }

        // One more tick to ensure reset to idle
        world.tick();
        {
            const InputAction& attack = Inputs::getAction(registry, userId, "attack");
            const InputAction& jump = Inputs::getAction(registry, userId, "jump");

            MANI_TEST_ASSERT(!attack.pressed() && !attack.held() && !attack.released(), "attack idle again");
            MANI_TEST_ASSERT(!jump.pressed() && !jump.held() && !jump.released(), "jump idle again");
        }
    }

    MANI_TEST(ResolveHint, "Should resolve input hints to valid control ids")
    {
        Application app;
        World& world = app.getWorld();
        world.initialize();
        world.createSystem<InputSystem>();

        ECS::Registry& registry = world.getMutableRegistry();
        VirtualController controller(registry);
        const ECS::EntityId userId = registry.create();
        registry.add<InputUser>(userId);
        Inputs::assignDevice(registry, userId, controller.getDeviceId());

        // Resolve known hints
        ControlId aHint = Inputs::resolveHint(registry, userId, EInputHints::Gamepad_Bottom_FaceButton);
        ControlId bHint = Inputs::resolveHint(registry, userId, EInputHints::Gamepad_Right_FaceButton);
        ControlId dpadUpHint = Inputs::resolveHint(registry, userId, EInputHints::Gamepad_DPad_Up);

        MANI_TEST_ASSERT(aHint == controller.aButtonId, "resolved A button correctly");
        MANI_TEST_ASSERT(bHint == controller.bButtonId, "resolved B button correctly");
        MANI_TEST_ASSERT(dpadUpHint == controller.dpadUpId, "resolved DPad up correctly");
    }

    MANI_TEST(BindActionWithHint, "Should bind and respond to an action using a hint instead of explicit control id")
    {
        Application app;
        World& world = app.getWorld();
        world.initialize();
        world.createSystem<InputSystem>();

        ECS::Registry& registry = world.getMutableRegistry();
        VirtualController controller(registry);
        const ECS::EntityId userId = registry.create();
        registry.add<InputUser>(userId);
        Inputs::assignDevice(registry, userId, controller.getDeviceId());

        // Add and bind action via hint
        Inputs::addAction(registry, userId, "shoot", EInputHints::Gamepad_Bottom_FaceButton);

        // Should not yet be pressed
        world.tick();
        MANI_TEST_ASSERT(!Inputs::getAction(registry, userId, "shoot").pressed(), "not pressed initially");

        // Press A button → should resolve via hint
        controller.press(controller.aButtonId);
        world.tick();
        MANI_TEST_ASSERT(Inputs::getAction(registry, userId, "shoot").pressed(), "pressed via hint binding");

        // Release → should show as released
        controller.release(controller.aButtonId);
        world.tick();
        MANI_TEST_ASSERT(Inputs::getAction(registry, userId, "shoot").released(), "released via hint binding");
    }

    MANI_TEST(UnbindActionWithHint, "Should stop responding after unbinding by hint")
    {
        Application app;
        World& world = app.getWorld();
        world.initialize();
        world.createSystem<InputSystem>();

        ECS::Registry& registry = world.getMutableRegistry();
        VirtualController controller(registry);
        const ECS::EntityId userId = registry.create();
        registry.add<InputUser>(userId);
        Inputs::assignDevice(registry, userId, controller.getDeviceId());

        Inputs::addAction(registry, userId, "block", EInputHints::Gamepad_Right_FaceButton);

        controller.press(controller.bButtonId);
        world.tick();
        MANI_TEST_ASSERT(Inputs::getAction(registry, userId, "block").pressed(), "pressed before unbind");

        Inputs::unbindAction(registry, userId, "block", EInputHints::Gamepad_Right_FaceButton);
        controller.release(controller.bButtonId);
        world.tick();

        // Press again — should not trigger anymore
        controller.press(controller.bButtonId);
        world.tick();
        MANI_TEST_ASSERT(!Inputs::getAction(registry, userId, "block").pressed(), "no response after unbind");
    }

    MANI_TEST(BindActionAxisWithHint, "Should bind an axis direction using input hints")
    {
        Application app;
        World& world = app.getWorld();
        world.initialize();
        world.createSystem<InputSystem>();

        ECS::Registry& registry = world.getMutableRegistry();
        VirtualController controller(registry);
        const ECS::EntityId userId = registry.create();
        registry.add<InputUser>(userId);
        Inputs::assignDevice(registry, userId, controller.getDeviceId());

        Inputs::addAction(registry, userId, "move");
        Inputs::bindActionAxis(registry, userId, "move", EInputAxis::Up, EInputHints::Gamepad_DPad_Up);
        Inputs::bindActionAxis(registry, userId, "move", EInputAxis::Down, EInputHints::Gamepad_DPad_Down);

        controller.press(controller.dpadUpId);
        world.tick();
        MANI_TEST_ASSERT(Inputs::getAction(registry, userId, "move").y > 0.0f, "move up positive via hint");

        controller.release(controller.dpadUpId);
        controller.press(controller.dpadDownId);
        world.tick();
        MANI_TEST_ASSERT(Inputs::getAction(registry, userId, "move").y < 0.0f, "move down negative via hint");
    }

    MANI_TEST(AddActionWithHint, "Should add and bind in one call using hint")
    {
        Application app;
        World& world = app.getWorld();
        world.initialize();
        world.createSystem<InputSystem>();

        ECS::Registry& registry = world.getMutableRegistry();
        VirtualController controller(registry);
        const ECS::EntityId userId = registry.create();
        registry.add<InputUser>(userId);
        Inputs::assignDevice(registry, userId, controller.getDeviceId());

        Inputs::addAction(registry, userId, "jump", EInputHints::Gamepad_Bottom_FaceButton);

        controller.press(controller.aButtonId);
        world.tick();

        MANI_TEST_ASSERT(Inputs::getAction(registry, userId, "jump").pressed(), "auto-bound action responds correctly");

        controller.release(controller.aButtonId);
        world.tick();
        MANI_TEST_ASSERT(Inputs::getAction(registry, userId, "jump").released(), "released correctly");
    }

    MANI_TEST(AddActionWithHintBeforeDeviceAssignment, "Should add an action with hint before assigning device, then respond correctly after assignment")
    {
        Application app;
        World& world = app.getWorld();
        world.initialize();
        world.createSystem<InputSystem>();

        ECS::Registry& registry = world.getMutableRegistry();
        VirtualController controller(registry);

        const ECS::EntityId userId = registry.create();
        registry.add<InputUser>(userId);

        // Step 1: Add action with hint BEFORE assigning device
        Inputs::addAction(registry, userId, "shoot", EInputHints::Gamepad_Bottom_FaceButton);

        // Step 2: Verify that action exists but is not pressed
        MANI_TEST_ASSERT(!Inputs::getAction(registry, userId, "shoot").pressed(), "action exists but not pressed before device assignment");

        // Step 3: Assign the virtual controller device
        Inputs::assignDevice(registry, userId, controller.getDeviceId());

        // Step 4: Press the button corresponding to the hint
        controller.press(controller.aButtonId);
        world.tick();

        // Step 5: Verify the action responds
        MANI_TEST_ASSERT(Inputs::getAction(registry, userId, "shoot").pressed(), "action responds after device assignment");

        // Step 6: Release button and verify released state
        controller.release(controller.aButtonId);
        world.tick();
        MANI_TEST_ASSERT(Inputs::getAction(registry, userId, "shoot").released(), "action released correctly after device assignment");
    }
}
MANI_SECTION_END(Inputs)