#include "simpleTests.h"
#include <Core/World/World.h>
#include <Core/System/SystemContainer.h>
#include <Events/Event.h>

using namespace Mani;

namespace Mani_Test
{
	class SomeSystem : public SystemBase
	{
	public:
		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override
		{
			onInitializeCalled = true;
		}

		virtual void onDeinitialize(EntityRegistry& registry) override
		{
			if (onDeinitializeCalled)
			{
				onDeinitializeCalled();
			}
		}

		virtual bool shouldTick(EntityRegistry& registry) const override
		{
			return true;
		}

		virtual void tick(float deltaTime, EntityRegistry& registry) override
		{
			tickCalled = true;
		}

		void assignOnDeinitializeCalled(std::function<void()> f)
		{
			onDeinitializeCalled = f;
		}

		bool onInitializeCalled = false;
		bool tickCalled = false;
		std::function<void()> onDeinitializeCalled;
	};
}

ST_SECTION_BEGIN(Core_World, "Core World")
{
	ST_TEST(CreateAndInitializeAWorld, "should create and initialize a world with a single system.")
	{	
		using namespace Mani_Test;
		
		World world;
		world.initialize();
		SystemContainer& systemContainer = world.getSystemContainer();

		systemContainer.createSystem<SomeSystem>();
		ST_ASSERT(!systemContainer.getSystem<SomeSystem>().expired(), "Should return true when creating a system.");

		std::shared_ptr<SomeSystem> someSystem = world.getSystemContainer().getSystem<SomeSystem>().lock();
		ST_ASSERT(someSystem != nullptr, "Should get a non nullptr system");
		if (someSystem == nullptr)
		{
			return;
		}

		ST_ASSERT(someSystem->onInitializeCalled, "onInitialize should have been called.");

		world.tick(.16f);
	
		ST_ASSERT(someSystem->tickCalled, "tick should have been called.");

		bool onDeinitializeCalled = false;
		someSystem->assignOnDeinitializeCalled([&onDeinitializeCalled]() 
		{
			onDeinitializeCalled = true;
		});

		world.getSystemContainer().destroySystem<SomeSystem>();
		ST_ASSERT(systemContainer.getSystem<SomeSystem>().expired(), "Should return true when destroying a system.");
		ST_ASSERT(onDeinitializeCalled, "onDeinitialize should have been called.");

		world.getSystemContainer().destroySystem<SomeSystem>();
		world.deinitialize();
	}

	ST_TEST(InitializationOrder, "Should respect the flow of initialization when creating a world and its systems")
	{
		using namespace Mani_Test;

		World world;
		SystemContainer& systemContainer = world.getSystemContainer();

		systemContainer.createSystem<SomeSystem>();
		std::shared_ptr<SomeSystem> someSystem = systemContainer.getSystem<SomeSystem>().lock();
		ST_ASSERT(someSystem != nullptr, "SomeSystem should not be null");
		if (someSystem == nullptr)
		{
			return;
		}

		ST_ASSERT(!someSystem->onInitializeCalled, "System initialize should not have been called yet");
		systemContainer.tick(.16f);
		ST_ASSERT(!someSystem->tickCalled, "system tick should not have been called yet, since the system isn't initialized");

		bool onDeinitializeCalled = false;
		someSystem->assignOnDeinitializeCalled([&onDeinitializeCalled]() 
		{
			onDeinitializeCalled = true;
		});

		systemContainer.deinitialize();
		ST_ASSERT(!onDeinitializeCalled, "System should not have been deinitialized, since it was not initialized in the first place.");

		systemContainer.initialize();
	
		ST_ASSERT(someSystem->onInitializeCalled, "System initialize should have been called");
		systemContainer.tick(.16f);
		ST_ASSERT(someSystem->tickCalled, "System tick have been called, since the system is now initialized");
		systemContainer.deinitialize();
		ST_ASSERT(onDeinitializeCalled, "System should have been deinitialized, since it was initialized.");

		world.getSystemContainer().destroySystem<SomeSystem>();
		world.deinitialize();
	}

	ST_TEST(DeinitializationOrder, "Should deinitialize systems in the reversed order they were intialized")
	{
		class InitHandle
		{
		public:
			int id;
			InitHandle()
			{
				static int s_count = 0;
				id = s_count++;
			};
		};
		
		DECLARE_EVENT(OnDeinitializedEvent, );
		
		class SomeSystem1 : public SystemBase
		{
		public:
			virtual std::string_view getName() const override { return "SomeSystem1"; }
			virtual void onDeinitialize(EntityRegistry& registry) override { onDeinitialized.broadcast(); }
			InitHandle initHandle;
			OnDeinitializedEvent onDeinitialized;
		};

		class SomeSystem2 : public SystemBase
		{
		public:
			virtual std::string_view getName() const override { return "SomeSystem2"; }
			virtual void onDeinitialize(EntityRegistry& registry) override { onDeinitialized.broadcast(); }
			InitHandle initHandle;
			OnDeinitializedEvent onDeinitialized;
		};

		class SomeSystem3 : public SystemBase
		{
		public:
			virtual std::string_view getName() const override { return "SomeSystem3"; }
			virtual void onDeinitialize(EntityRegistry& registry) override { onDeinitialized.broadcast(); }
			InitHandle initHandle;
			OnDeinitializedEvent onDeinitialized;
		};

		World world;
		SystemContainer& systemContainer = world.getSystemContainer();

		systemContainer.createSystem<SomeSystem1>();
		systemContainer.createSystem<SomeSystem2>();
		systemContainer.createSystem<SomeSystem3>();

		std::shared_ptr<SomeSystem1> someSystem1 = systemContainer.getSystem<SomeSystem1>().lock();
		std::shared_ptr<SomeSystem2> someSystem2 = systemContainer.getSystem<SomeSystem2>().lock();
		std::shared_ptr<SomeSystem3> someSystem3 = systemContainer.getSystem<SomeSystem3>().lock();

		world.initialize();

		ST_ASSERT(someSystem1->initHandle.id == 0, "Should be the first one to be initialized");
		ST_ASSERT(someSystem2->initHandle.id == 1, "Should be the second one to be initialized");
		ST_ASSERT(someSystem3->initHandle.id == 2, "Should be the third one to be initialized");

		bool wasOnDeinitCalled = false;
		int onDeinitCounter = 0;
		someSystem1->onDeinitialized.subscribe([&wasOnDeinitCalled, &onDeinitCounter]()
		{
			wasOnDeinitCalled = true;
			ST_ASSERT(onDeinitCounter == 2, "Should be the third one to be deinitialized");
			onDeinitCounter++;
		});
		
		someSystem2->onDeinitialized.subscribe([&wasOnDeinitCalled, &onDeinitCounter]()
		{
			wasOnDeinitCalled = true;
			ST_ASSERT(onDeinitCounter == 1, "Should be the second one to be deinitialized");
			onDeinitCounter++;
		});
		
		someSystem3->onDeinitialized.subscribe([&wasOnDeinitCalled, &onDeinitCounter]()
		{
			wasOnDeinitCalled = true;
			ST_ASSERT(onDeinitCounter == 0, "Should be the first one to be deinitialized");
			onDeinitCounter++;
		});

		world.deinitialize();
		ST_ASSERT(wasOnDeinitCalled, "OnDeinit should have been called")
	}

	ST_TEST(HandleSystemInheritance, "Should handle inheritance")
	{
		using namespace Mani_Test;

		class SomeExtendedSystem : public SomeSystem 
		{
		public:
			virtual std::string_view getName() const override { return "SomeExtendedSystem"; }
		};

		World world;
		SystemContainer& systemContainer = world.getSystemContainer();
		systemContainer.createSystem<SomeExtendedSystem>();
		if (systemContainer.getSystem<SomeExtendedSystem>().expired())
		{
			ST_ASSERT(false, "did not create the system, should have created the system");
			return;
		}

		std::shared_ptr<SomeSystem> someSystem = systemContainer.getSystem<SomeSystem>().lock();
		ST_ASSERT(someSystem != nullptr, "should retrieve the base system");
		if (someSystem == nullptr)
		{
			return;
		}

		size_t sizeBefore = systemContainer.size();
		ST_ASSERT(someSystem->getName() == "SomeExtendedSystem", "Should return the extended name");
		systemContainer.createSystem<SomeSystem>();
		ST_ASSERT(sizeBefore == systemContainer.size(), "should not have allowed creating a system of type SomeSystem");

		world.getSystemContainer().destroySystem<SomeExtendedSystem>();
		world.deinitialize();
	}

	ST_TEST(SystemDependencyFlow, "Should create 2 systems with a dependency relationship")
	{
		class SomeSystem : public SystemBase
		{
		public:
			virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override
			{
				onInitializeCalled = true;
			}

			bool onInitializeCalled = false;
		};

		class SomeOtherSystem : public SystemBase
		{
		public:
			virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override
			{
				onInitializeCalled = true;
				dependency = systemContainer.initializeDependency<SomeSystem>();
			
				ST_ASSERT(!dependency.expired(), "SomeSystem should have been initialized.");
				if (dependency.expired())
				{
					return;
				}

				ST_ASSERT(dependency.lock()->onInitializeCalled, "SomeSystem should have been initialized.");
			}

			bool onInitializeCalled = false;
			std::weak_ptr<SomeSystem> dependency;
		};

		World world;
		SystemContainer& systemContainer = world.getSystemContainer();
		systemContainer.initialize();
		systemContainer.createSystem<SomeOtherSystem>();
		if (systemContainer.getSystem<SomeOtherSystem>().expired())
		{
			ST_ASSERT(false, "did not create the system, should have created the system");
			return;
		}

		std::shared_ptr<SomeOtherSystem> someOtherSystem = systemContainer.getSystem<SomeOtherSystem>().lock();
		if (someOtherSystem == nullptr)
		{
			ST_ASSERT(false, "did not create the system, should have created the system");
			return;
		}

		ST_ASSERT(someOtherSystem->onInitializeCalled, "SomeOtherSystem should have been initialized.");

		std::weak_ptr<SomeSystem> someSystem = systemContainer.getSystem<SomeSystem>();
		if (someSystem.expired())
		{
			ST_ASSERT(false, "did not create the system, should have created the system");
			return;
		}

		ST_ASSERT(someSystem.lock()->onInitializeCalled, "someSystem should have been initialized.");

		systemContainer.destroySystem<SomeSystem>();
		ST_ASSERT(someOtherSystem->dependency.expired(), "SomeSystem should have been destroyed and its shared_ptr reset.");

		world.getSystemContainer().destroySystem<SomeOtherSystem>();
		world.deinitialize();
	}

	ST_TEST(DependencyInjection, "Should be able to refer to systems by interface")
	{
		class ISomeFunctionality
		{
		public:
			virtual void DoFunctionality() = 0;
		};

		class SomeCustomSystem : public SystemBase, public ISomeFunctionality
		{
		public:
			virtual void DoFunctionality() override
			{
				ST_ASSERT(true, "Nice functionality!");
			}
		};

		World world;
		SystemContainer& systemContainer = world.getSystemContainer();

		systemContainer.createSystem<SomeCustomSystem>();
		std::shared_ptr<ISomeFunctionality> someFunctionalitySystem = systemContainer.getSystem<ISomeFunctionality>().lock();
		ST_ASSERT(someFunctionalitySystem != nullptr, "Should have been able to access SomeSystem");
		
		someFunctionalitySystem->DoFunctionality();
	}
}
ST_SECTION_END(Core_World)