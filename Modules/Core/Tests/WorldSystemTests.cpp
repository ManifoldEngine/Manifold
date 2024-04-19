#include "simpleTests.h"
#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <Core/World/WorldSystem.h>
#include <Events/Event.h>

using namespace Mani;

ST_SECTION_BEGIN(WorldSystemSection, "WorldSytem")
{
	ST_TEST(WorldSystemCreate, "Should create an Application and setup a world")
	{
		bool hasTicked = false;
		class SomeWorldSystem : public SystemBase
		{
		public:
			virtual std::string_view getName() const override { return "SomeWorldSystem"; }
			virtual bool shouldTick(EntityRegistry& registry) const override { return true; }

			virtual void tick(float deltaTime, EntityRegistry& registry) override
			{
				onTick.broadcast(deltaTime);
			}

			DECLARE_EVENT(OnTickEvent, float /*deltatime*/);
			OnTickEvent onTick;
		};

		Application app;
		app.getSystemContainer().initialize();
		std::shared_ptr<WorldSystem> worldSystem = app.getSystemContainer().getSystem<WorldSystem>().lock();
		std::shared_ptr<World> world = worldSystem->createWorld();
		
		if (world == nullptr)
		{
			ST_ASSERT(false, "World should not be null");
			return;
		}

		ST_ASSERT(world->isInitialized(), "World shoudl be initialized");
		
		std::shared_ptr<SomeWorldSystem> someWorlSystem = world->getSystemContainer().initializeDependency<SomeWorldSystem>().lock();
		someWorlSystem->onTick.subscribe([&hasTicked](float deltaTime) 
		{ 
			ST_ASSERT(deltaTime < 1.192092896e-07F, "Should have ticked with the same delta time than the application's tick");
			hasTicked = true;
		});

		app.tick(0.f);
		ST_ASSERT(hasTicked, "World should have ticked.");
	}
}
ST_SECTION_END(WorldSystemSection)
