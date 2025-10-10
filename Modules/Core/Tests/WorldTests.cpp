#include <Core/World.h>
#include <Core/ECS/System.h>

#include <ManiTests/ManiTests.h>

using namespace Mani;

namespace Mani_Test
{
	class SomeSystem : public ECS::System
	{
	public:
		virtual void onInitialize(ECS::Registry& registry, World& world) override
		{
			onInitializeCalled = true;
		}

		virtual void onDeinitialize(ECS::Registry& registry, World& world) override
		{
			if (onDeinitializeCalled)
			{
				onDeinitializeCalled();
			}
		}

		virtual bool shouldTick(const ECS::Registry& registry) const override
		{
			return true;
		}

		virtual void tick(ECS::Registry& registry) override
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

		world.tick();
	
		MANI_TEST_ASSERT(SomeSystem::tickCalled, "tick should have been called.");

		bool onDeinitializeCalled = false;
		SomeSystem::onDeinitializeCalled = [&onDeinitializeCalled]()
		{
			onDeinitializeCalled = true;
		};

		world.destroySystem<SomeSystem>();
		MANI_TEST_ASSERT(!world.has<SomeSystem>(), "Should not return true when destroying a system.");
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
		world.tick();
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
		world.tick();
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

		class SomeSystem1 : public ECS::System
		{
		public:
			virtual std::string_view getName() const override { return "SomeSystem1"; }
			virtual void onInitialize(ECS::Registry& registry, World& world) override { someSystem1Initialized = true; }
			virtual void onDeinitialize(ECS::Registry& registry, World& world) override { someSystem1DeInitialized = true; }
		};

		class SomeSystem2 : public ECS::System
		{
		public:
			virtual std::string_view getName() const override { return "SomeSystem2"; }
			virtual void onInitialize(ECS::Registry& registry, World& world) override { someSystem2Initialized = true; }
			virtual void onDeinitialize(ECS::Registry& registry, World& world) override { someSystem2DeInitialized = true; }
		};

		class SomeSystem3 : public ECS::System
		{
		public:
			virtual std::string_view getName() const override { return "SomeSystem3"; }
			virtual void onInitialize(ECS::Registry& registry, World& world) override { someSystem3Initialized = true; }
			virtual void onDeinitialize(ECS::Registry& registry, World& world) override { someSystem3DeInitialized = true; }
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
		MANI_TEST_ASSERT(sizeBefore + 1 == world.systemCount(), "should have allowed creating a system of type SomeSystem");

		world.destroySystem<SomeExtendedSystem>();
		world.deinitialize();
	}

	MANI_TEST(SystemDependencyFlow, "Should create 2 systems with a dependency relationship")
	{
		static bool someSystemInitialized = false;
		static bool someOtherSystemInitialized = false;

		class SomeSystem : public ECS::System
		{
		public:
			virtual void onInitialize(ECS::Registry& registry, World& world) override
			{
				someSystemInitialized = true;
			}
		};

		class SomeOtherSystem : public ECS::System
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

	MANI_TEST(ShouldBeAbleToMixupSystemCreationAccrossMultipleTickGroups, "Should Be Able To Mixup System Creation Accross Multiple TickGroups")
	{
		struct TickOrder { std::vector<std::string> order; };
		
		class SomePreUpdateSystem :		public ECS::System { public: virtual ETickGroup getTickGroup() const override { return ETickGroup::PreUpdate;	} virtual std::string_view getName() const override { return "SomePreUpdateSystem";	}	virtual void tick(ECS::Registry& registry) override { registry.getSingle<TickOrder>()->order.push_back(getName().data()); } virtual bool shouldTick(const ECS::Registry& registry) const override { return true; }};
		class SomeUpdateSystem :		public ECS::System { public: virtual ETickGroup getTickGroup() const override { return ETickGroup::Update;		} virtual std::string_view getName() const override { return "SomeUpdateSystem";	}	virtual void tick(ECS::Registry& registry) override { registry.getSingle<TickOrder>()->order.push_back(getName().data()); } virtual bool shouldTick(const ECS::Registry& registry) const override { return true; }};
		class SomePostUpdateSystem :	public ECS::System { public: virtual ETickGroup getTickGroup() const override { return ETickGroup::PostUpdate;	} virtual std::string_view getName() const override { return "SomePostUpdateSystem";}	virtual void tick(ECS::Registry& registry) override { registry.getSingle<TickOrder>()->order.push_back(getName().data()); } virtual bool shouldTick(const ECS::Registry& registry) const override { return true; }};
		class SomePreRenderSystem :		public ECS::System { public: virtual ETickGroup getTickGroup() const override { return ETickGroup::PreRender;	} virtual std::string_view getName() const override { return "SomePreRenderSystem";	}	virtual void tick(ECS::Registry& registry) override { registry.getSingle<TickOrder>()->order.push_back(getName().data()); } virtual bool shouldTick(const ECS::Registry& registry) const override { return true; }};
		class SomeRenderSystem :		public ECS::System { public: virtual ETickGroup getTickGroup() const override { return ETickGroup::Render;		} virtual std::string_view getName() const override { return "SomeRenderSystem";	}	virtual void tick(ECS::Registry& registry) override { registry.getSingle<TickOrder>()->order.push_back(getName().data()); } virtual bool shouldTick(const ECS::Registry& registry) const override { return true; }};
		class SomePostRenderSystem :	public ECS::System { public: virtual ETickGroup getTickGroup() const override { return ETickGroup::PostRender; }  virtual std::string_view getName() const override { return "SomePostRenderSystem";}	virtual void tick(ECS::Registry& registry) override { registry.getSingle<TickOrder>()->order.push_back(getName().data()); } virtual bool shouldTick(const ECS::Registry& registry) const override { return true; }};

		const std::vector<std::string> expected = 
		{
			"SomePreUpdateSystem",
			"SomeUpdateSystem",
			"SomePostUpdateSystem",
			"SomePreRenderSystem",
			"SomeRenderSystem",
			"SomePostRenderSystem",
		};

		{
			World world;
			world.initialize();
			ECS::Registry& registry = world.getMutableRegistry();
			registry.addSingle<TickOrder>();

			world.createSystem<SomePostRenderSystem>();
			world.createSystem<SomeRenderSystem>();
			world.createSystem<SomePreRenderSystem>();
			world.createSystem<SomePostUpdateSystem>();
			world.createSystem<SomeUpdateSystem>();
			world.createSystem<SomePreUpdateSystem>();

			world.tick();

			const TickOrder tickOrder1 = *registry.getSingle<TickOrder>();
			MANI_TEST_ASSERT(tickOrder1.order == expected, "order should have been respected");
		}

		{
			World world;
			world.initialize();
			ECS::Registry& registry = world.getMutableRegistry();
			registry.addSingle<TickOrder>();

			world.createSystems<SomePostRenderSystem,
								SomeRenderSystem,
								SomePreRenderSystem,
								SomePostUpdateSystem,
								SomeUpdateSystem,
								SomePreUpdateSystem>();

			world.tick();

			const TickOrder tickOrder1 = *registry.getSingle<TickOrder>();
			MANI_TEST_ASSERT(tickOrder1.order == expected, "order should have been respected");
		}
	}

	MANI_SECTION_BEGIN(SystemSets, "System Sets")
	{
		MANI_TEST(CreateAndDestroySystemSet, "Should create then destroy all systems in the same variadic template type")
		{
			static size_t createdSytems = 0;
			static size_t destroyedSystems = 0;

			class MyFirstSystem : public Mani::ECS::System
			{
			protected:
				virtual void onInitialize(Mani::ECS::Registry& registry, Mani::World& world) override
				{
					createdSytems++;
				}

				virtual void onDeinitialize(Mani::ECS::Registry& registry, Mani::World& world) override
				{
					destroyedSystems++;
				}
			};

			class MySecondSystem : public Mani::ECS::System
			{
			protected:
				virtual void onInitialize(Mani::ECS::Registry& registry, Mani::World& world) override
				{
					createdSytems++;
				}
				virtual void onDeinitialize(Mani::ECS::Registry& registry, Mani::World& world) override
				{
					destroyedSystems++;
				}
			};

			{
				World world;
				world.initialize();
				world.createSystems<MyFirstSystem, MySecondSystem>();
				const bool hasSystems = world.hasSystems<MyFirstSystem, MySecondSystem>();
				MANI_ASSERT(hasSystems, "should have the system set");
				world.destroySystems<MyFirstSystem, MySecondSystem>();
				MANI_ASSERT(createdSytems == 2, "should have created a set of 2 systems");
				MANI_ASSERT(destroyedSystems == 2, "should have destroyed a set of 2 systems");
			}

			createdSytems = 0;
			destroyedSystems = 0;
			using MySystemSet = Mani::TypeList<MyFirstSystem, MySecondSystem>;

			{
				World world;
				world.initialize();
				world.createSystems(MySystemSet{});
				MANI_ASSERT(world.hasSystems(MySystemSet{}), "should have the system set");
				world.destroySystems(MySystemSet{});
				MANI_ASSERT(createdSytems == 2, "should have created a set of 2 systems");
				MANI_ASSERT(destroyedSystems == 2, "should have destroyed a set of 2 systems");
			}
		}
	}
	MANI_SECTION_END(SystemSets)
}
MANI_SECTION_END(Core_World)