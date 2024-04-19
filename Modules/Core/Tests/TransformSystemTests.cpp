#include "simpleTests.h"
#include <Core/CoreFwd.h>

using namespace Mani;

ST_SECTION_BEGIN(TransformSystemSection, "TransformSystem")
{
	class TestSystemBase : public SystemBase
	{
	public:
		virtual std::string_view getName() const override { return "TestSystem"; }
		virtual bool shouldTick(EntityRegistry& registry) const override { return true; }

		EntityId parentId;
		EntityId childId;

		// this is just for the test, you should never ever do this.
		Transform* parentTransform = nullptr;
		Transform* childTransform = nullptr;

	protected:
		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer) override
		{
			parentId = registry.create();
			childId = registry.create();
			
			registry.addComponent<Transform>(parentId);
			registry.addComponent<Transform>(childId);

			parentTransform = registry.getComponent<Transform>(parentId);
			childTransform = registry.getComponent<Transform>(childId);

			childTransform->parentId = parentId;
			childTransform->localPosition = glm::vec3(5.f, 0.f, 0.f);
		}

		virtual void onDeinitialize(EntityRegistry& registry) override
		{
			registry.destroy(childId);
			registry.destroy(parentId);
		}
	};

	ST_TEST(UpdateTransformLocalPosition, "Should properly parent two transforms")
	{
		class TestSystem : public TestSystemBase
		{
			virtual void tick(float deltaTime, EntityRegistry& registry) override
			{
				Transform* parentTransform = registry.getComponent<Transform>(parentId);
				parentTransform->position += glm::vec3(0.f, 5.f, 0.f);
			}
		};

		World world;
		world.initialize();

		SystemContainer& systemContainer = world.getSystemContainer();
		std::shared_ptr<TestSystem> testSystem = systemContainer.initializeDependency<TestSystem>().lock();

		world.tick(.16f);

		ST_ASSERT(glm::length2(testSystem->parentTransform->position - glm::vec3(0.f, 5.f, 0.f)) <= FLT_EPSILON, "parent transform should be at (0, 5, 0)");
		ST_ASSERT(glm::length2(testSystem->childTransform->position - glm::vec3(5.f, 5.f, 0.f)) <= FLT_EPSILON, "child transform should be at (5, 5, 0)");

		world.tick(.16f);

		ST_ASSERT(glm::length2(testSystem->parentTransform->position - glm::vec3(0.f, 10.f, 0.f)) <= FLT_EPSILON, "parent transform should be at (0, 10, 0)");
		ST_ASSERT(glm::length2(testSystem->childTransform->position - glm::vec3(5.f, 10.f, 0.f)) <= FLT_EPSILON, "child transform should be at (5, 10, 0)");
	}
}
ST_SECTION_END(TransformSystemSection)