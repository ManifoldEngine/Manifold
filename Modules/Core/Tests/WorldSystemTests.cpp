#include <Core/System/SystemContainer.h>
#include <Core/Application.h>
#include <Core/World/WorldSystem.h>
#include <Events/Event.h>

#include <ManiTests/ManiTests.h>

using namespace Mani;

MANI_SECTION_BEGIN(WorldSystemSection, "WorldSytem")
{
	MANI_TEST(WorldSystemCreate, "Should create an Application and setup a world")
	{
		class SomeWorldSystem : public SystemBase
		{
		public:
			virtual std::string_view getName() const override { return "SomeWorldSystem"; }
			virtual bool shouldTick(ECS::Registry& registry) const override { return true; }

			virtual void tick(float deltaTime, ECS::Registry& registry) override
			{
				hasTicked = true;
			}

			bool hasTicked = false;
		};

		class SomeAppSystem : public SystemBase
		{
		public:
			virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer)
			{
				m_worldId = WorldSystem::createWorld(registry);
				m_world = registry.get<World>(m_worldId);

				if (m_world == nullptr)
				{
					MANI_TEST_ASSERT(false, "World should not be null");
					return;
				}

				m_world->systemContainer.createSystem<SomeWorldSystem>();
				hasBeenInitialized = true;
			}

			virtual void onDeinitialize(ECS::Registry& registry)
			{
				WorldSystem::destroyWorld(registry, m_worldId);
			}

			// don't do this, this is for brievety's sake
			ECS::EntityId m_worldId; 
			World* m_world = nullptr;
			bool hasBeenInitialized = false;
		};

		Application app;
		std::shared_ptr<SomeAppSystem> sysApp = app.getSystemContainer().initializeDependency<SomeAppSystem>().lock();
		MANI_TEST_ASSERT(sysApp->hasBeenInitialized == true, "should intialized SomeAppSystem");
		
		app.tick(0.f);

		std::shared_ptr<SomeWorldSystem> sysWorld = sysApp->m_world->systemContainer.getSystem<SomeWorldSystem>().lock();
		MANI_TEST_ASSERT(sysWorld->hasTicked, "World should have ticked.");
	}
}
MANI_SECTION_END(WorldSystemSection)
