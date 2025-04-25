#include <Core/Application.h>
#include <Core/Async/Parallel.h>
#include <Core/ECS/Registry.h>
#include <Core/ECS/System.h>

#include <ManiTests/ManiTests.h>

#include <functional>
#include <iostream>

using namespace Mani;

MANI_SECTION_BEGIN(Async, "Async")
{
	MANI_TEST(ExecParallelFor, "Should iterate over entities in parallel")
	{
		struct MyComponent
		{
			int value = 0;
		};

		constexpr int ENTITYCOUNT = 956;

		Application app;
		ECS::Registry registry;
		for (int i = 0; i < ENTITYCOUNT; ++i)
		{ 
			const ECS::EntityId entityId = registry.create();
			registry.add<MyComponent>(entityId);
		}

		ECS::View<MyComponent> view(registry);
		Mani::parallelFor(view, [&registry](ECS::EntityId entityId, size_t threadIndex)
		{
			MyComponent& comp = *registry.get<MyComponent>(entityId);
			comp.value += 10;
		});

		int iterationCount = 0;
		for (const ECS::EntityId entityId : view)
		{
			iterationCount++;
			const MyComponent& comp = *registry.get<MyComponent>(entityId);
			MANI_TEST_ASSERT(comp.value == 10, "value should be 10");
		}
		MANI_TEST_ASSERT(iterationCount == ENTITYCOUNT, "should have iterated.");
	}

	MANI_TEST(ExecDefer, "Should defer a function after the a tick.")
	{
		enum class ECallers : uint8_t
		{
			Deferred,
			System,
		};
		
		struct Arbiter
		{
			std::vector<ECallers> callers;
		};

		class TestSystem : public ECS::System
		{
		public:
			virtual bool shouldTick(ECS::Registry&) const override { return true; }
			virtual void tick(float deltaTime, ECS::Registry& registry)
			{
				Arbiter& arbiter = *registry.getSingle<Arbiter>();
				arbiter.callers.push_back(ECallers::System);
			}
		};

		Application app;
		app.getWorld().createSystem<TestSystem>();
		ECS::Registry& registry = app.getWorld().getMutableRegistry();
		registry.addSingle<Arbiter>();

		Mani::defer([&registry] {
			Arbiter& arbiter = *registry.getSingle<Arbiter>();
			arbiter.callers.push_back(ECallers::Deferred);
		});

		app.tick(0.16f);

		Arbiter* arbiter = registry.getSingle<Arbiter>();
		MANI_TEST_ASSERT(arbiter != nullptr, "arbiter should have been added");
		const std::vector<ECallers> expected = { ECallers::System, ECallers::Deferred };
		MANI_TEST_ASSERT(arbiter->callers == expected, "deferred function should have run after system's tick");
	}
}
MANI_SECTION_END(Thread)