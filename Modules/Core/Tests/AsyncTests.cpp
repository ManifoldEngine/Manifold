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
		const SizeT minChunkSize = ENTITYCOUNT / Application::THREAD_COUNT;
		Mani::parallelFor(view, [](ECS::EntityId entityId, MyComponent& myComp)
		{
			myComp.value += 10;
		}, minChunkSize);

		int iterationCount = 0;
		for (auto [entityId, myComp] : view)
		{
			iterationCount++;
			MANI_ASSERT(myComp.value == 10, "value should be 10");
		}
		MANI_TEST_ASSERT(iterationCount == ENTITYCOUNT, "should have iterated.");
	}

	MANI_TEST(ExecParallelForWithConstView, "Should iterate over entities in parallel with a const view")
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
			registry.add<MyComponent>(entityId, 69);
		}

		std::atomic<int> counter{ 0 };
		ECS::ConstView<MyComponent> view(registry);
		const SizeT minChunkSize = ENTITYCOUNT / Application::THREAD_COUNT;
		Mani::parallelFor(view, [&counter](ECS::EntityId entityId, const MyComponent& myComp)
		{
			// incredibly slow btw
			counter++;
			MANI_TEST_ASSERT(myComp.value == 69, "Should be equal.");
		}, minChunkSize);

		MANI_TEST_ASSERT(counter == ENTITYCOUNT, "should have iterated.");
	}

	MANI_TEST(ExecParallelForWithConstViewAndUsageOfCommandBuffer, "Should iterate over entities in parallel with a const view and usage of command buffer")
	{
		struct MyComponent
		{
			int value = 0;
		};

		struct MyOtherComponent
		{
			int value = 0;
		};

		constexpr int ENTITYCOUNT = 148;

		Application app;
		ECS::Registry registry;
		for (int i = 0; i < ENTITYCOUNT; ++i)
		{
			const ECS::EntityId entityId = registry.create();
			registry.add<MyComponent>(entityId, 69);
		}

		{
			ECS::View<MyComponent> view(registry);
			const SizeT minChunkSize = ENTITYCOUNT / Application::THREAD_COUNT;
			Mani::parallelForWithCmd(view, [](ECS::CommandBuffer& cmd, ECS::EntityId entityId, const MyComponent& myComp)
			{
				cmd.add<MyOtherComponent>(entityId, 420);
			}, minChunkSize);
		}

		{
			int counter = 0;
			ECS::ConstView<MyComponent, MyOtherComponent> view(registry);
			for (const auto [entity, myComp, myOtherComp] : view)
			{
				counter++;
				MANI_TEST_ASSERT(myComp.value == 69, "Should still be equal");
				MANI_TEST_ASSERT(myOtherComp.value == 420, "New component should have correct value");
			}
			MANI_TEST_ASSERT(counter == ENTITYCOUNT, "should have iterated.");
		}
	}

	MANI_TEST(ParallelForOnOneElement, "Should iterate over entities in parallel with a view of one element")
	{
		struct MyComponent
		{
			int value = 0;
		};

		constexpr int ENTITYCOUNT = 1;

		Application app;
		ECS::Registry registry;
		for (int i = 0; i < ENTITYCOUNT; ++i)
		{
			const ECS::EntityId entityId = registry.create();
			registry.add<MyComponent>(entityId, 69);
		}

		{
			ECS::View<MyComponent> view(registry);
			constexpr SizeT minChunkSize = 1;
			Mani::parallelFor(view, [](ECS::EntityId entityId, MyComponent& myComp)
			{
				myComp.value += 420;
			}, minChunkSize);
		}

		{
			int counter = 0;
			ECS::ConstView<MyComponent> view(registry);
			for (const auto [entity, myComp] : view)
			{
				counter++;
				MANI_TEST_ASSERT(myComp.value == 69 + 420, "Should still be equal");
			}
			MANI_TEST_ASSERT(counter == ENTITYCOUNT, "should have iterated.");
		}
	}

	MANI_TEST(ParallelForOn10Elements, "Should iterate over entities in parallel with a view of 10 elements")
	{
		struct MyComponent
		{
			int value = 0;
		};

		constexpr int ENTITYCOUNT = 10;

		Application app;
		ECS::Registry registry;
		for (int i = 0; i < ENTITYCOUNT; ++i)
		{
			const ECS::EntityId entityId = registry.create();
			registry.add<MyComponent>(entityId, 69);
		}

		{
			ECS::View<MyComponent> view(registry);
			constexpr SizeT minChunkSize = 1;
			Mani::parallelFor(view, [](ECS::EntityId entityId, MyComponent& myComp)
			{
				myComp.value += 420;
			}, minChunkSize);
		}

		{
			int counter = 0;
			ECS::ConstView<MyComponent> view(registry);
			for (const auto [entity, myComp] : view)
			{
				counter++;
				MANI_TEST_ASSERT(myComp.value == 69 + 420, "Should still be equal");
			}
			MANI_TEST_ASSERT(counter == ENTITYCOUNT, "should have iterated.");
		}
	}

	MANI_TEST(ParallelForWithThreadIndex, "Should iterate over entities in parallel with a view of 10 elements")
	{
		struct MyComponent
		{
			SizeT value = 0;
		};

		constexpr int ENTITYCOUNT = 10;

		Application app;
		ECS::Registry registry;
		for (int i = 0; i < ENTITYCOUNT; ++i)
		{
			const ECS::EntityId entityId = registry.create();
			registry.add<MyComponent>(entityId, 69);
		}

		{
			ECS::View<MyComponent> view(registry);
			constexpr SizeT minChunkSize = 1;
			Mani::parallelFor(view, [](SizeT threadIdx, ECS::EntityId entityId, MyComponent& myComp)
			{
				myComp.value = threadIdx;
			}, 1);
		}

		{
			int counter = 0;
			ECS::ConstView<MyComponent> view(registry);
			for (const auto [entity, myComp] : view)
			{
				MANI_TEST_ASSERT(myComp.value == counter, "Should still be equal");
				counter++;
			}
			MANI_TEST_ASSERT(counter == ENTITYCOUNT, "should have iterated.");
		}
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
			virtual bool shouldTick(const ECS::Registry&) const override { return true; }
			virtual void tick(ECS::Registry& registry)
			{
				Ref<Arbiter> arbiter = registry.getSingle<Arbiter>();
				arbiter->callers.push_back(ECallers::System);
			}
		};

		Application app;
		app.getWorld().createSystem<TestSystem>();
		ECS::Registry& registry = app.getWorld().getMutableRegistry();
		registry.addSingle<Arbiter>();

		Mani::defer([&registry] {
			Ref<Arbiter> arbiter = registry.getSingle<Arbiter>();
			arbiter->callers.push_back(ECallers::Deferred);
		});

		app.tick();

		Ref<Arbiter> arbiter = registry.getSingle<Arbiter>();
		const std::vector<ECallers> expected = { ECallers::System, ECallers::Deferred };
		MANI_TEST_ASSERT(arbiter->callers == expected, "deferred function should have run after system's tick");
	}


	MANI_TEST(Tick50Times, "App Should tick 50 times")
	{
		Application app;
		for (int i = 0; i < 50; i++)
		{
			app.tick();
		}
	}

	MANI_TEST(HandleLargeAmountOfQueuedTask, "Should be able to handle a large amount of tasks")
	{
		constexpr size_t TASK_AMOUNT = 100;
		constexpr size_t WORK_AMOUNT = 1000;

		Application app;
		ThreadPool& threadPool = app.getThreadPool();

		std::latch latch{ TASK_AMOUNT };
		std::atomic<int> result = 0;
		for (size_t i = 0; i < TASK_AMOUNT; i++)
		{
			threadPool.enqueue([&result, &latch, WORK_AMOUNT] 
			{
				for (size_t i = 0; i < WORK_AMOUNT; i++)
				{
					result++;
				}
				latch.count_down();
			});
		}

		latch.wait();
		MANI_TEST_ASSERT(result == TASK_AMOUNT * WORK_AMOUNT, "All world should have bee completed");
	}
}
MANI_SECTION_END(Thread)