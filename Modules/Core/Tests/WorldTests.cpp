#include <Core/World/World.h>
#include <Core/System/System.h>
#include <Events/Event.h>

#include <ManiTests/ManiTests.h>

using namespace Mani;

namespace Mani_Test
{
	class SomeSystem : public SystemBase
	{
	public:
		virtual void onInitialize(ECS::Registry& registry, World& world) override
		{
			onInitializeCalled = true;
		}

		virtual void onDeinitialize(ECS::Registry& registry) override
		{
			if (onDeinitializeCalled)
			{
				onDeinitializeCalled();
			}
		}

		virtual bool shouldTick(ECS::Registry& registry) const override
		{
			return true;
		}

		virtual void tick(float deltaTime, ECS::Registry& registry) override
		{
			tickCalled = true;
		}

		inline static bool onInitializeCalled = false;
		inline static bool tickCalled = false;
		inline static std::function<void()> onDeinitializeCalled;
	};
}

MANI_SECTION_BEGIN(Core_World, "Core World")
{
	MANI_BEFORE_EACH(Core_WorldBeforeEach)
	{
		using namespace Mani_Test;

		SomeSystem::onInitializeCalled = false;
		SomeSystem::tickCalled = false;
		SomeSystem::onDeinitializeCalled = [](){};
	}

	MANI_TEST(CreateAndInitializeAWorld, "should create and initialize a world with a single system.")
	{	
		using namespace Mani_Test;
		
		World world;
		world.initialize();

		world.createSystem<SomeSystem>();
		MANI_TEST_ASSERT(world.has<SomeSystem>(), "Should return true when creating a system.");

		
		MANI_TEST_ASSERT(SomeSystem::onInitializeCalled, "onInitialize should have been called.");

		world.tick(.16f);
	
		MANI_TEST_ASSERT(SomeSystem::tickCalled, "tick should have been called.");

		bool onDeinitializeCalled = false;
		SomeSystem::onDeinitializeCalled = [&onDeinitializeCalled]()
		{
			onDeinitializeCalled = true;
		};

		world.destroySystem<SomeSystem>();
		MANI_TEST_ASSERT(!world.has<SomeSystem>(), "Should return true when destroying a system.");
		MANI_TEST_ASSERT(onDeinitializeCalled, "onDeinitialize should have been called.");

		world.destroySystem<SomeSystem>();
		world.deinitialize();
	}

	MANI_TEST(InitializationOrder, "Should respect the flow of initialization when creating a world and its systems")
	{
		using namespace Mani_Test;

		World world;

		world.createSystem<SomeSystem>();
		
		MANI_TEST_ASSERT(!SomeSystem::onInitializeCalled, "System initialize should not have been called yet");
		world.tick(.16f);
		MANI_TEST_ASSERT(!SomeSystem::tickCalled, "system tick should not have been called yet, since the system isn't initialized");

		bool onDeinitializeCalled = false;
		SomeSystem::onDeinitializeCalled = [&onDeinitializeCalled]()
		{
			onDeinitializeCalled = true;
		};

		world.deinitialize();
		MANI_TEST_ASSERT(!onDeinitializeCalled, "System should not have been deinitialized, since it was not initialized in the first place.");

		world.initialize();
	
		MANI_TEST_ASSERT(SomeSystem::onInitializeCalled, "System initialize should have been called");
		world.tick(.16f);
		MANI_TEST_ASSERT(SomeSystem::tickCalled, "System tick have been called, since the system is now initialized");
		world.deinitialize();
		MANI_TEST_ASSERT(onDeinitializeCalled, "System should have been deinitialized, since it was initialized.");

		world.destroySystem<SomeSystem>();
		world.deinitialize();
	}

	MANI_TEST(DeinitializationOrder, "Should deinitialize systems in the reversed order they were intialized")
	{
		static bool someSystem1Initialized = false;
		static bool someSystem2Initialized = false;
		static bool someSystem3Initialized = false;

		static bool someSystem1DeInitialized = false;
		static bool someSystem2DeInitialized = false;
		static bool someSystem3DeInitialized = false;

		DECLARE_EVENT(OnDeinitializedEvent, );
		
		class SomeSystem1 : public SystemBase
		{
		public:
			virtual std::string_view getName() const override { return "SomeSystem1"; }
			virtual void onInitialize(ECS::Registry& registry, World& world) override { someSystem1Initialized = true; }
			virtual void onDeinitialize(ECS::Registry& registry) override { someSystem1DeInitialized = true; }
		};

		class SomeSystem2 : public SystemBase
		{
		public:
			virtual std::string_view getName() const override { return "SomeSystem2"; }
			virtual void onInitialize(ECS::Registry& registry, World& world) override { someSystem2Initialized = true; }
			virtual void onDeinitialize(ECS::Registry& registry) override { someSystem2DeInitialized = true; }
		};

		class SomeSystem3 : public SystemBase
		{
		public:
			virtual std::string_view getName() const override { return "SomeSystem3"; }
			virtual void onInitialize(ECS::Registry& registry, World& world) override { someSystem3Initialized = true; }
			virtual void onDeinitialize(ECS::Registry& registry) override { someSystem3DeInitialized = true; }
		};

		World world;

		world.createSystem<SomeSystem1>();
		world.createSystem<SomeSystem2>();
		world.createSystem<SomeSystem3>();

		world.initialize();

		MANI_TEST_ASSERT(someSystem1Initialized, "Should be the first one to be initialized");
		MANI_TEST_ASSERT(someSystem2Initialized, "Should be the second one to be initialized");
		MANI_TEST_ASSERT(someSystem3Initialized, "Should be the third one to be initialized");

		MANI_TEST_ASSERT(!someSystem1DeInitialized, "Should not have been deinitialized");
		MANI_TEST_ASSERT(!someSystem2DeInitialized, "Should not have been deinitialized");
		MANI_TEST_ASSERT(!someSystem3DeInitialized, "Should not have been deinitialized");

		world.deinitialize();

		MANI_TEST_ASSERT(someSystem1DeInitialized, "Should have been deinitialized");
		MANI_TEST_ASSERT(someSystem2DeInitialized, "Should have been deinitialized");
		MANI_TEST_ASSERT(someSystem3DeInitialized, "Should have been deinitialized");
	}

	MANI_TEST(HandleSystemInheritance, "Should handle inheritance")
	{
		using namespace Mani_Test;

		class SomeExtendedSystem : public SomeSystem 
		{
		public:
			virtual std::string_view getName() const override { return "SomeExtendedSystem"; }
		};

		World world;
		world.createSystem<SomeExtendedSystem>();
		if (!world.has<SomeExtendedSystem>())
		{
			MANI_TEST_ASSERT(false, "did not create the system, should have created the system");
			return;
		}

		size_t sizeBefore = world.systemCount();
		world.createSystem<SomeSystem>();
		MANI_TEST_ASSERT(sizeBefore == world.systemCount(), "should not have allowed creating a system of type SomeSystem");

		world.destroySystem<SomeExtendedSystem>();
		world.deinitialize();
	}

	MANI_TEST(SystemDependencyFlow, "Should create 2 systems with a dependency relationship")
	{
		static bool someSystemInitialized = false;
		static bool someOtherSystemInitialized = false;

		class SomeSystem : public SystemBase
		{
		public:
			virtual void onInitialize(ECS::Registry& registry, World& world) override
			{
				someSystemInitialized = true;
			}
		};

		class SomeOtherSystem : public SystemBase
		{
		public:
			virtual void onInitialize(ECS::Registry& registry, World& world) override
			{
				someOtherSystemInitialized = true;
				world.initializeDependency<SomeSystem>();
				MANI_TEST_ASSERT(someSystemInitialized, "SomeSystem should have been initialized.");
			}
		};

		World world;
		world.initialize();
		world.createSystem<SomeOtherSystem>();
		if (!world.has<SomeOtherSystem>())
		{
			MANI_TEST_ASSERT(false, "did not create the system, should have created the system");
			return;
		}

		if (!world.has<SomeOtherSystem>())
		{
			MANI_TEST_ASSERT(false, "did not create the system, should have created the system");
			return;
		}

		MANI_TEST_ASSERT(someSystemInitialized, "someSystem should have been initialized.");
		MANI_TEST_ASSERT(someOtherSystemInitialized, "SomeOtherSystem should have been initialized.");

		world.destroySystem<SomeSystem>();
		world.destroySystem<SomeOtherSystem>();
		world.deinitialize();
	}
}
MANI_SECTION_END(Core_World)