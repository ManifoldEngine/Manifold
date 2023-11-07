#include "simpleTests.h"
#include "ECS/EntityRegistry.h"
#include "ECS/RegistryView.h"

using namespace ECSEngine;

ST_SECTION_BEGIN(ECS, "ECS")
{
	ST_AFTER_EACH(AfdterEachECSTest)
	{
		EntityRegistry::resetComponentIds();
	}

	ST_TEST(CreateEditDestroyEntities, "Should create a registry, create an entity, add a component, edit the component, then destroy the entity")
	{
		struct DataComponent 
		{
			int someData = 5;
		};

		// create a registry
		EntityRegistry registry;

		// create an entity
		EntityId entityId = registry.create();
	
		ST_ASSERT(entityId == 0, "Entity should have the first id.");
		ST_ASSERT(registry.isValid(entityId), "Entity should be valid.");
	
		// add a DataComponent
		auto* component = registry.addComponent<DataComponent>(entityId);
	
		ST_ASSERT(component != nullptr, "Component should not be a nullptr.");
		if (component == nullptr)
		{
			return;
		}

		// edit the data
		component->someData += 10;

		// get the component again to verify the pointer points to the correct location.
		auto* componentFromRegistry = registry.getComponent<DataComponent>(entityId);
	
		ST_ASSERT(componentFromRegistry != nullptr, "Component should not be a nullptr.");
		if (componentFromRegistry == nullptr)
		{
			return;
		}

		ST_ASSERT(component->someData == componentFromRegistry->someData, "We should be able to retrieve our component data");
	
		// destroy the entity, should destroy the component too.
		ST_ASSERT(registry.destroy(entityId), "The Entity should have been destroyed");
	
		ST_ASSERT(!registry.isValid(entityId), "Entity should not be valid anymore");

		auto* componentFromRegistryAfterDelete = registry.getComponent<DataComponent>(entityId);
		ST_ASSERT(componentFromRegistryAfterDelete == nullptr, "Entity's component should be nullptr after delete.");
	}

	ST_TEST(AddAndRemoveComponents, "Should create a registry and an entity, then add and remove a component")
	{
		struct DataComponent
		{
			int someData = 5;
		};

		// create a registry
		EntityRegistry registry;

		// create an entity
		EntityId entityId = registry.create();
	
		ST_ASSERT(entityId == 0, "Should be the first entity addded.");

		// add a DataComponent
		DataComponent* dataComponent = registry.addComponent<DataComponent>(entityId);
		ST_ASSERT(dataComponent != nullptr, "Component should not be a nullptr.");
		if (dataComponent == nullptr)
		{
			return;
		}

		const bool didRemoveDataComponent = registry.removeComponent<DataComponent>(entityId);
		ST_ASSERT(didRemoveDataComponent, "Should have removed the Data component");

		dataComponent = registry.getComponent<DataComponent>(entityId);
		ST_ASSERT(dataComponent == nullptr, "Component should be a nullptr.");
	}

	ST_TEST(RecycleEntities, "Should create a registry and an entity, then properly recycle the entity and its components")
	{
		struct DataComponent
		{
			int someData = 5;
		};

		struct OtherDataComponent {
			int someOtherData = 10;
		};

		// create a registry
		EntityRegistry registry;

		// create an entity
		EntityId entityId = registry.create();

		ST_ASSERT(entityId == 0, "Should be the first entity addded.");

		// add a DataComponent
		DataComponent* dataComponent = registry.addComponent<DataComponent>(entityId);
		ST_ASSERT(dataComponent != nullptr, "Component should not be a nullptr.");
		if (dataComponent == nullptr)
		{
			return;
		}

		// destroy the entity, which should be added to the entity pool
		const bool didDestroy = registry.destroy(entityId);
		ST_ASSERT(didDestroy, "Should have detroyed entity");

		dataComponent = registry.getComponent<DataComponent>(entityId);
		ST_ASSERT(dataComponent == nullptr, "Component should be a nullptr.");

		// we expect the entity to be recycled
		entityId = registry.create();
		ST_ASSERT(entityId == 0, "Should be the first entity addded.");

		// Entity should not have a DataComponent
		dataComponent = registry.getComponent<DataComponent>(entityId);
		ST_ASSERT(dataComponent == nullptr, "Component should be a nullptr.");

		OtherDataComponent* otherDataComponent = registry.addComponent<OtherDataComponent>(entityId);
		ST_ASSERT(otherDataComponent != nullptr, "Component should not be a nullptr.");
		if (otherDataComponent == nullptr)
		{
			return;
		}

		otherDataComponent->someOtherData += 10;

		OtherDataComponent* otherOtherDataComponent = registry.getComponent<OtherDataComponent>(entityId);
		ST_ASSERT(otherOtherDataComponent != nullptr, "Other Component should not be a nullptr.");
		if (otherOtherDataComponent == nullptr)
		{
			return;
		}

		ST_ASSERT(otherOtherDataComponent->someOtherData == otherDataComponent->someOtherData, "we should be able to mutate a component of a recycled entity.");
	}

	ST_TEST(CreateRegistryView, "Should create a registry view and iterate through it")
	{
		struct DataComponent {
			int someData = 5;
		};

		struct OtherDataComponent {
			int someOtherData = 10;
		};

		// create a registry
		EntityRegistry registry;

		// create e few entities, all of them with DataComponent, some of them with OtherDataComponent
		const int entityCount = 10;
		for (int i = 0; i < entityCount; i++)
		{
			const EntityId entityId = registry.create();
			registry.addComponent<DataComponent>(entityId);
			if (i % 2 == 0)
			{
				registry.addComponent<OtherDataComponent>(entityId);
			}
		}

		ST_ASSERT(registry.size() == entityCount, "should have created 10 entities");

		// iterate over the entities with DataComponent
		int entityCounter = 0;
		for (EntityId entityId : RegistryView<DataComponent>(registry))
		{
			entityCounter++;
			auto* component = registry.getComponent<DataComponent>(entityId);
			ST_ASSERT(component != nullptr, "should have a DataComponent");
		}
		ST_ASSERT(entityCounter == entityCount, "There should be 10 entities with DataComponent in the registry view");

		entityCounter = 0;
		for (EntityId entityId : RegistryView<OtherDataComponent>(registry))
		{
			entityCounter++;
			auto* otherComponent = registry.getComponent<OtherDataComponent>(entityId);
			ST_ASSERT(otherComponent != nullptr, "should have a OtherDataComponent");
		}
		ST_ASSERT(entityCounter == entityCount / 2, "There should be 5 entities with OtherDataComponent in the registry view");

		entityCounter = 0;
		for (EntityId entityId : RegistryView<DataComponent, OtherDataComponent>(registry))
		{
			entityCounter++;

			auto* component = registry.getComponent<DataComponent>(entityId);
			ST_ASSERT(component != nullptr, "should have a DataComponent");

			auto* otherComponent = registry.getComponent<OtherDataComponent>(entityId);
			ST_ASSERT(otherComponent != nullptr, "should have a OtherDataComponent");
		}
		ST_ASSERT(entityCounter == entityCount / 2, "There should be 5 entities with DataComponent and OtherDataComponent in the registry view");

		entityCounter = 0;
		for (EntityId entityId : RegistryView<>(registry))
		{
			entityCounter++;
		}
		ST_ASSERT(entityCounter == 10, "Should iterate over all entities");
	}

	ST_TEST(ChaoticEntityCreation, "We should be able to create entities, some without component, and some with components")
	{
		struct DataComponent {
			int someData = 5;
		};

		// create a registry
		EntityRegistry registry;

		// create e few entities, all of them with DataComponent, some of them with OtherDataComponent
		const int entityCount = 10;
		for (int i = 0; i < entityCount; i++)
		{
			const EntityId entityId = registry.create();
			if (i % 2 == 0)
			{
				registry.addComponent<DataComponent>(entityId);
			}
		}

		ST_ASSERT(registry.size() == entityCount, "should have created 10 entities");

		// iterate over the entities with DataComponent
		int entityCounter = 0;
		for (EntityId entityId : RegistryView<DataComponent>(registry))
		{
			entityCounter++;
			auto* component = registry.getComponent<DataComponent>(entityId);
			ST_ASSERT(component != nullptr, "should have a DataComponent");
		}
		ST_ASSERT(entityCounter == entityCount / 2, "There should be 5 entities with DataComponent in the registry view");
	}

	ST_TEST(ComponentIdAssignation, "ComponentIds should be consecutive")
	{	
		const ComponentId componentId0 = EntityRegistry::getComponentId<struct Component0>();
		const ComponentId componentId1 = EntityRegistry::getComponentId<struct Component1>();
		const ComponentId componentId2 = EntityRegistry::getComponentId<struct Component2>();
		
		ST_ASSERT(componentId1 - componentId0 == 1, "The second componentid should follow the first one");
		ST_ASSERT(componentId2 - componentId1 == 1, "The third componentid should follow the second one");
	}

	ST_TEST(Spawn1000000Entities, "Should spawn 1'000'000 entities with a transform")
	{
		struct Transform
		{
			float position[3];
			float rotation[3];
			float scale[3];
		};

		EntityRegistry registry;
		for (int i = 0; i < 1'000'000; ++i)
		{
			const EntityId entityId = registry.create();
			Transform* transform = registry.addComponent<Transform>(entityId);
			transform->position[0] = 1.f;
			transform->position[1] = 1.f;
			transform->position[2] = 1.f;

			transform->rotation[0] = 1.f;
			transform->rotation[1] = 1.f;
			transform->rotation[2] = 1.f;

			transform->scale[0] = 1.f;
			transform->scale[1] = 1.f;
			transform->scale[2] = 1.f;
		}

		ST_ASSERT(registry.size() == 1'000'000, "Should have spawned 1'000'000 entities.");
	}
}
ST_SECTION_END(ECS)