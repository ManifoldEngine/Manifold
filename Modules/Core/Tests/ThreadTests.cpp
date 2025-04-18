#include <Core/Application.h>
#include <Core/Thread/Parallel.h>
#include <Core/ECS/Registry.h>

#include <ManiTests/ManiTests.h>

#include <functional>
#include <iostream>

using namespace Mani;

MANI_SECTION_BEGIN(Thread, "Thread")
{
	MANI_TEST(ParallelFor, "Should iterate over entities in parallel")
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
}
MANI_SECTION_END(Thread)