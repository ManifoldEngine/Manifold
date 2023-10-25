#ifdef ECSE_TEST

#include "simpleTests.h"
#include "World.h"

using namespace ECSEngine;

ST_TEST(CreateAndInitializeAWorld, "should create and initialize a world with a single system.")
{
	class SomeSystem : public SystemBase
	{
	public:
		virtual void onInitialize(EntityRegistry& registry) override
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
	
	World world;

	bool bDidCreateSystem = world.createSystem<SomeSystem>();
	ST_ASSERT(bDidCreateSystem, "Should return true when creating a system.");

	bDidCreateSystem = world.createSystem<SomeSystem>();
	ST_ASSERT(!bDidCreateSystem, "Should not return true when creating a system of a type that already exists.");

	auto* pSomeSystem = world.getSystem<SomeSystem>();
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

	bool bDidDestroySystem = world.destroySystem<SomeSystem>();
	ST_ASSERT(bDidDestroySystem, "Should return true when destroying a system.");
	ST_ASSERT(bOnDeinitializeCalled, "onDeinitialize should have been called.");
}

#endif