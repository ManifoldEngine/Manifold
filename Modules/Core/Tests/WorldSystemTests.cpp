#include <Core/Application.h>
#include <Core/System/SystemContainer.h>
#include <Core/World/WorldSystem.h>
#include <Events/Event.h>

#include <ManiTests/ManiTests.h>

using namespace Mani;

MANI_SECTION_BEGIN(WorldSystemSection, "WorldSytem")
{
	MANI_TEST(WorldSystemCreate, "Should create an Application and setup a world")
	{
		bool hasTicked = false;
		class SomeWorldSystem : public SystemBase
		{
		public:
			virtual std::string_view getName() const override { return "SomeWorldSystem"; }
			virtual bool shouldTick(ECS::Registry& registry) const override { return true; }

			virtual void tick(float deltaTime, ECS::Registry& registry) override
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
			MANI_TEST_ASSERT(false, "World should not be null");
			return;
		}

		MANI_TEST_ASSERT(world->isInitialized(), "World shoudl be initialized");
		
		std::shared_ptr<SomeWorldSystem> someWorlSystem = world->getSystemContainer().initializeDependency<SomeWorldSystem>().lock();
		someWorlSystem->onTick.subscribe([&hasTicked](float deltaTime) 
		{ 
			MANI_TEST_ASSERT(deltaTime < 1.192092896e-07F, "Should have ticked with the same delta time than the application's tick");
			hasTicked = true;
		});

		app.tick(0.f);
		MANI_TEST_ASSERT(hasTicked, "World should have ticked.");
	}
}
MANI_SECTION_END(WorldSystemSection)
