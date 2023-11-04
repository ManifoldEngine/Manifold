#include "simpleTests.h"
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <Core/World/WorldSystem.h>
#include <Events/Event.h>

using namespace ECSEngine;

ST_SECTION_BEGIN(WorldSystemSection, "WorldSytem")
{
	ST_TEST(WorldSystemCreate, "Should create an Application and setup a world")
	{
		bool bHasTicked = false;
		class SomeWorldSystem : public SystemBase
		{
		public:
			virtual std::string_view getName() const override { return "SomeWorldSystem"; }
			virtual bool shouldTick(EntityRegistry& registry) const override { return true; }

			virtual void tick(float deltaTime, EntityRegistry& registry)
			{
				onTick.broadcast(deltaTime);
			}

			DECLARE_EVENT(OnTickEvent, float /*deltatime*/);
			OnTickEvent onTick;
		};

		Application app;
		app.getSystemContainer().initialize();
		std::shared_ptr<WorldSystem> pWorldSystem = app.getSystemContainer().getSystem<WorldSystem>().lock();
		std::shared_ptr<World> pWorld = pWorldSystem->createWorld();
		
		if (pWorld == nullptr)
		{
			ST_ASSERT(false, "World should not be null");
			return;
		}

		ST_ASSERT(pWorld->isInitialized(), "World shoudl be initialized");
		
		std::shared_ptr<SomeWorldSystem> pSomeWorlSystem = pWorld->getSystemContainer().initializeDependency<SomeWorldSystem>().lock();
		pSomeWorlSystem->onTick.subscribe([&bHasTicked](float deltaTime) { 
			ST_ASSERT(deltaTime < FLT_EPSILON, "Should have ticked with the same delta time than the application's tick");
			bHasTicked = true;
		});

		app.tick(0.f);
		ST_ASSERT(bHasTicked, "World should have ticked.");
	}
}