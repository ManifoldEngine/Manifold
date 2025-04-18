#include <Core/Application.h>
#include <Core/World/WorldSystem.h>
#include <Events/Event.h>

#include <ManiTests/ManiTests.h>

using namespace Mani;

MANI_SECTION_BEGIN(WorldSystemSection, "WorldSytem")
{
	MANI_TEST(WorldSystemCreate, "Should create an Application and setup a world")
	{
		static bool SomeWorldSystemTicked = false;
		

		class SomeWorldSystem : public ECS::System
		{
		public:
			virtual std::string_view getName() const override { return "SomeWorldSystem"; }
			virtual bool shouldTick(ECS::Registry& registry) const override { return true; }

			virtual void tick(float deltaTime, ECS::Registry& registry) override
			{
				SomeWorldSystemTicked = true;
			}
		};

		static bool SomeAppSystemInitialized = false;

		class SomeAppSystem : public ECS::System
		{
		public:
			virtual void onInitialize(ECS::Registry& registry, World& world)
			{
				m_worldId = WorldSystem::createWorld(registry);
				m_world = registry.get<World>(m_worldId);

				if (m_world == nullptr)
				{
					MANI_TEST_ASSERT(false, "World should not be null");
					return;
				}

				m_world->createSystem<SomeWorldSystem>();
				SomeAppSystemInitialized = true;
			}

			virtual void onDeinitialize(ECS::Registry& registry)
			{
				WorldSystem::destroyWorld(registry, m_worldId);
			}

			// don't do this, this is for brievety's sake
			ECS::EntityId m_worldId; 
			World* m_world = nullptr;
		};

		Application app;
		app.getWorld().createSystem<SomeAppSystem>();
		MANI_TEST_ASSERT(SomeAppSystemInitialized, "should intialized SomeAppSystem");
		
		app.tick(0.f);

		MANI_TEST_ASSERT(SomeWorldSystemTicked, "World should have ticked.");
	}
}
MANI_SECTION_END(WorldSystemSection)
