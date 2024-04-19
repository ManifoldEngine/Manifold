#include "simpleTests.h"
#include "ECS/EntityRegistry.h"
#include "ECS/RegistryView.h"
#include "ECS/Bitset.h"

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	SimpleTests::SimpleTestsRunner::runTests();
}
#endif

using namespace Mani;

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

		int entityCounter = 0;
		for (EntityId entityId : RegistryView<DataComponent, OtherDataComponent>(registry))
		{
			entityCounter++;

			auto* component = registry.getComponent<DataComponent>(entityId);
			ST_ASSERT(component != nullptr, "should have a DataComponent");

			auto* otherComponent = registry.getComponent<OtherDataComponent>(entityId);
			ST_ASSERT(otherComponent != nullptr, "should have a OtherDataComponent");
		}
		ST_ASSERT(entityCounter == entityCount / 2, "There should be 5 entities with DataComponent and OtherDataComponent in the registry view");

		// iterate over the entities with DataComponent
		entityCounter = 0;
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
		struct Component0 {};
		struct Component1 {};
		struct Component2 {};

		EntityRegistry registry;
		const ComponentId componentId0 = registry.getComponentId<Component0>();
		const ComponentId componentId1 = registry.getComponentId<Component1>();
		const ComponentId componentId2 = registry.getComponentId<Component2>();
		
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

	ST_TEST(PlayWithDynamicAllocation, "Should allow dynamic allocation")
	{
		struct DynamicComponent
		{
			std::vector<float> vector = { 1.f, 2.f, 3.f };
		};

		EntityRegistry registry;
		const EntityId entity1 = registry.create();
		const EntityId entity2 = registry.create();

		DynamicComponent* component1 = registry.addComponent<DynamicComponent>(entity1);
		DynamicComponent* component2 = registry.addComponent<DynamicComponent>(entity2);
		for (int i = 0; i < 1000; ++i)
		{
			component1->vector.push_back(i + 1 * 1000.f);
		}

		ST_ASSERT(component2->vector[0] - 1.f <= 1.192092896e-07F, "first vector element should be equal to the original value");
		ST_ASSERT(component2->vector[1] - 2.f <= 1.192092896e-07F, "second vector element should be equal to the original value");
		ST_ASSERT(component2->vector[2] - 3.f <= 1.192092896e-07F, "third vector element should be equal to the original value");
	}

	ST_TEST(EntityHasComponent, "Should return true if the entity has the component or not")
	{
		struct Component {};

		EntityRegistry registry;

		const EntityId entityId = registry.create();

		ST_ASSERT(registry.hasComponent<Component>(entityId) == false, "Should not have the component yet");

		registry.addComponent<Component>(entityId);

		ST_ASSERT(registry.hasComponent<Component>(entityId) == true, "Should now have the component");
	}

	ST_TEST(EntityRegistryEvents, "Should subscribe to all events and confirm that a callback was received")
	{
		struct Component {};

		EntityRegistry registry;

		std::vector<EntityId> entityIdCreated;
		std::vector<EntityId> entityIdDestroyed;
		std::vector<std::pair<EntityId, ComponentId>> componentIdCreated;
		std::vector<std::pair<EntityId, ComponentId>> componentIdDestroyed;

		registry.onEntityCreated.subscribe([&entityIdCreated](EntityRegistry& registry, EntityId entityId) {
			entityIdCreated.push_back(entityId);
		});

		registry.onEntityDestroyed.subscribe([&entityIdDestroyed](EntityRegistry& registry, EntityId entityId) {
			entityIdDestroyed.push_back(entityId);
		});

		registry.onComponentAdded.subscribe([&componentIdCreated](EntityRegistry& registry, EntityId entityId, ComponentId componentId) {
			componentIdCreated.push_back({ entityId, componentId });
		});

		registry.onComponentRemoved.subscribe([&componentIdDestroyed](EntityRegistry& registry, EntityId entityId, ComponentId componentId) {
			componentIdDestroyed.push_back({ entityId, componentId });
		});

		EntityId entityId = registry.create();
		ComponentId componentId = registry.getComponentId<Component>();
		registry.addComponent<Component>(entityId);
		registry.removeComponent<Component>(entityId);
		registry.destroy(entityId);

		std::pair<EntityId, ComponentId> pair = { entityId, componentId };
		ST_ASSERT(entityIdCreated.size() == 1 && entityIdCreated[0] == entityId, "EntityId should have been created");
		ST_ASSERT((componentIdCreated.size() == 1 && componentIdCreated[0] == pair), "ComponentId should have been added to EntityId");
		ST_ASSERT((componentIdDestroyed.size() == 1 && componentIdDestroyed[0] == pair), "ComponentId should have been removed from EntityId");
		ST_ASSERT(entityIdDestroyed.size() == 1 && entityIdDestroyed[0] == entityId, "EntityId should have been destroyed");
	}

	ST_TEST(BitsetTests, "Should set, test and reset a bitset properly")
	{
		Bitset<64> bitset;
		ST_ASSERT(bitset.any() == false, "fresh bitset should not be set");
		bitset.set(0);
		ST_ASSERT(bitset.test(0) == true, "bit 0 should have been set");
		ST_ASSERT(bitset.test(1) == false, "bit 0 should not have been set");
		bitset.reset();
		ST_ASSERT(bitset.any() == false, "fresh bitset should not be set");
		

		bitset.set(5);
		Bitset<64> otherBitset;
		otherBitset.set(5);
		ST_ASSERT(bitset == otherBitset, "bitsets should be equal");
		otherBitset.reset();
		ST_ASSERT(bitset != otherBitset, "Bitsets should not be equal anymore");

		bitset.reset();
		bitset.set(56);
		ST_ASSERT(bitset.test(56) == true, "bit 56 should have been set");

		bitset.reset();
		bitset.set(1);
		bitset.set(5);
		bitset.set(56);

		otherBitset.reset();
		otherBitset.set(1);
		otherBitset.set(5);
		otherBitset.set(56);

		ST_ASSERT(bitset == (bitset & otherBitset), "should be able to mask bitsets");

		bitset.reset();
		bitset.set(1);
		bitset.set(5);
		bitset.set(38);

		otherBitset.reset();
		otherBitset.set(4);
		otherBitset.set(15);
		otherBitset.set(64);

		Bitset<64> resultBitset;
		resultBitset.set(1);
		resultBitset.set(5);
		resultBitset.set(38);
		resultBitset.set(4);
		resultBitset.set(15);
		resultBitset.set(64);

		ST_ASSERT(resultBitset == (bitset | otherBitset), "should be able to add bitsets");
	}
}
ST_SECTION_END(ECS)