#include "simpleTests.h"
#include <Core/World.h>
#include <Core/System/SystemContainer.h>

using namespace ECSEngine;

namespace ECSEngine_Test
{
	class SomeSystem : public SystemBase
	{
	public:
		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override
		{
			bOnInitializeCalled = true;
		}

		virtual void onDeinitialize(EntityRegistry& registry) override
		{
			onDeinitializeCalled();
		}

		virtual bool shouldTick(EntityRegistry& registry) const override
		{
			return true;
		}

		virtual void tick(float deltaTime, EntityRegistry& registry) override
		{
			bTickCalled = true;
		}

		void assignOnDeinitializeCalled(std::function<void()> f)
		{
			onDeinitializeCalled = f;
		}

		bool bOnInitializeCalled = false;
		bool bTickCalled = false;
		std::function<void()> onDeinitializeCalled;
	};
}

ST_TEST(CreateAndInitializeAWorld, "should create and initialize a world with a single system.")
{	
	using namespace ECSEngine_Test;

	World world;

	world.getSystemContainer().initialize();

	bool bDidCreateSystem = world.getSystemContainer().createSystem<SomeSystem>();
	ST_ASSERT(bDidCreateSystem, "Should return true when creating a system.");

	bDidCreateSystem = world.getSystemContainer().createSystem<SomeSystem>();
	ST_ASSERT(!bDidCreateSystem, "Should not return true when creating a system of a type that already exists.");

	auto* pSomeSystem = world.getSystemContainer().getSystem<SomeSystem>();
	ST_ASSERT(pSomeSystem != nullptr, "Should get a non nullptr system");
	if (pSomeSystem == nullptr)
	{
		return;
	}

	ST_ASSERT(pSomeSystem->bOnInitializeCalled, "onInitialize should have been called.");

	world.tick(.16f);
	
	ST_ASSERT(pSomeSystem->bTickCalled, "tick should have been called.");

	bool bOnDeinitializeCalled = false;
	pSomeSystem->assignOnDeinitializeCalled([&bOnDeinitializeCalled]() {
		bOnDeinitializeCalled = true;
	});

	bool bDidDestroySystem = world.getSystemContainer().destroySystem<SomeSystem>();
	ST_ASSERT(bDidDestroySystem, "Should return true when destroying a system.");
	ST_ASSERT(bOnDeinitializeCalled, "onDeinitialize should have been called.");
}

ST_TEST(InitializationOrder, "Should respect the flow of initialization when creating a world and its systems")
{
	using namespace ECSEngine_Test;
	World world;
	SystemContainer& systemContainer = world.getSystemContainer();

	bool bDidCreateSystem = systemContainer.createSystem<SomeSystem>();
	ST_ASSERT(bDidCreateSystem, "Should return true when creating a system.");

	SomeSystem* pSomeSystem = systemContainer.getSystem<SomeSystem>();
	ST_ASSERT(pSomeSystem != nullptr, "SomeSystem should not be null");
	if (pSomeSystem == nullptr)
	{
		return;
	}

	ST_ASSERT(!pSomeSystem->bOnInitializeCalled, "System initialize should not have been called yet");
	systemContainer.tick(.16f);
	ST_ASSERT(!pSomeSystem->bTickCalled, "system tick should not have been called yet, since the system isn't initialized");

	bool bOnDeinitializeCalled = false;
	pSomeSystem->assignOnDeinitializeCalled([&bOnDeinitializeCalled]() {
		bOnDeinitializeCalled = true;
	});

	systemContainer.deinitialize();
	ST_ASSERT(!bOnDeinitializeCalled, "System should not have been deinitialized, since it was not initialized in the first place.");

	systemContainer.initialize();
	
	ST_ASSERT(pSomeSystem->bOnInitializeCalled, "System initialize should have been called");
	systemContainer.tick(.16f);
	ST_ASSERT(pSomeSystem->bTickCalled, "System tick have been called, since the system is now initialized");
	systemContainer.deinitialize();
	ST_ASSERT(bOnDeinitializeCalled, "System should have been deinitialized, since it was initialized.");
}

ST_TEST(HandleSystemInheritance, "Should handle inheritance")
{
	using namespace ECSEngine_Test;

	class SomeExtendedSystem : public SomeSystem 
	{
	public:
		virtual std::string_view getName() const override { return "SomeExtendedSystem"; }
	};

	World world;
	SystemContainer& systemContainer = world.getSystemContainer();
	if (!systemContainer.createSystem<SomeExtendedSystem>())
	{
		ST_ASSERT(false, "did not create the system, should have created the system");
		return;
	}

	auto* pSomeSystem = systemContainer.getSystem<SomeSystem>();
	ST_ASSERT(pSomeSystem != nullptr, "should retrieve the base system");
	if (pSomeSystem == nullptr)
	{
		return;
	}

	ST_ASSERT(pSomeSystem->getName() == "SomeExtendedSystem", "Should return the extended name");

	if (systemContainer.createSystem<SomeSystem>())
	{
		ST_ASSERT(false, "should not have allowed creating a system of type SomeSystem");
		return;
	}
}