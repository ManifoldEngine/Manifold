#include <Core/CoreFwd.h>
#include <ManiTests/ManiTests.h>

using namespace Mani;

MANI_SECTION_BEGIN(TransformSystemSection, "TransformSystem")
{
	class TestSystemBase : public SystemBase
	{
	public:
		virtual std::string_view getName() const override { return "TestSystem"; }
		virtual bool shouldTick(ECS::Registry& registry) const override { return true; }

		ECS::EntityId parentId;
		ECS::EntityId childId;

		// this is just for the test, you should never ever do this.
		Transform* parentTransform = nullptr;
		Transform* childTransform = nullptr;

	protected:
		virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer) override
		{
			parentId = registry.create();
			childId = registry.create();
			
			registry.add<Transform>(parentId);
			registry.add<Transform>(childId);

			parentTransform = registry.get<Transform>(parentId);
			childTransform = registry.get<Transform>(childId);

			childTransform->parentId = parentId;
			childTransform->localPosition = glm::vec3(5.f, 0.f, 0.f);
		}

		virtual void onDeinitialize(ECS::Registry& registry) override
		{
			registry.destroy(childId);
			registry.destroy(parentId);
		}
	};

	MANI_TEST(UpdateTransformLocalPosition, "Should properly parent two transforms")
	{
		class TestSystem : public TestSystemBase
		{
			virtual void tick(float deltaTime, ECS::Registry& registry) override
			{
				Transform* parentTransform = registry.get<Transform>(parentId);
				parentTransform->position += glm::vec3(0.f, 5.f, 0.f);
			}
		};

		World world;
		world.initialize();

		SystemContainer& systemContainer = world.getSystemContainer();
		std::shared_ptr<TestSystem> testSystem = systemContainer.initializeDependency<TestSystem>().lock();

		world.tick(.16f);

		MANI_TEST_ASSERT(glm::length2(testSystem->parentTransform->position - glm::vec3(0.f, 5.f, 0.f)) <= FLT_EPSILON, "parent transform should be at (0, 5, 0)");
		MANI_TEST_ASSERT(glm::length2(testSystem->childTransform->position - glm::vec3(5.f, 5.f, 0.f)) <= FLT_EPSILON, "child transform should be at (5, 5, 0)");

		world.tick(.16f);

		MANI_TEST_ASSERT(glm::length2(testSystem->parentTransform->position - glm::vec3(0.f, 10.f, 0.f)) <= FLT_EPSILON, "parent transform should be at (0, 10, 0)");
		MANI_TEST_ASSERT(glm::length2(testSystem->childTransform->position - glm::vec3(5.f, 10.f, 0.f)) <= FLT_EPSILON, "child transform should be at (5, 10, 0)");
	}
}
MANI_SECTION_END(TransformSystemSection)