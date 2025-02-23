#include <ManiTests/ManiTests.h>
#include <ECS/Registry.h>
#include <ECS/View.h>
#include <ECS/Bitset.h>

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	ManiTests::ManiTestsRunner::runTests();
}
#endif

using namespace Mani;

MANI_SECTION_BEGIN(ECS, "ECS")
{
	MANI_TEST(CreateEditDestroyEntities, "Should create a registry, create an entity, add a component, edit the component, then destroy the entity")
	{
		struct DataComponent 
		{
			int someData = 5;
		};

		// create a registry
		ECS::Registry registry;

		// create an entity
		ECS::EntityId entityId = registry.create();
	
		MANI_TEST_ASSERT(entityId == 1, "Entity should have the first id.");
		MANI_TEST_ASSERT(registry.isValid(entityId), "Entity should be valid.");
	
		// add a DataComponent
		auto* component = registry.add<DataComponent>(entityId);
	
		MANI_TEST_ASSERT(component != nullptr, "Component should not be a nullptr.");
		if (component == nullptr)
		{
			return;
		}

		// edit the data
		component->someData += 10;

		// get the component again to verify the pointer points to the correct location.
		auto* componentFromRegistry = registry.get<DataComponent>(entityId);
	
		MANI_TEST_ASSERT(componentFromRegistry != nullptr, "Component should not be a nullptr.");
		if (componentFromRegistry == nullptr)
		{
			return;
		}

		MANI_TEST_ASSERT(component->someData == componentFromRegistry->someData, "We should be able to retrieve our component data");
	
		// destroy the entity, should destroy the component too.
		MANI_TEST_ASSERT(registry.destroy(entityId), "The Entity should have been destroyed");
	
		MANI_TEST_ASSERT(!registry.isValid(entityId), "Entity should not be valid anymore");

		auto* componentFromRegistryAfterDelete = registry.get<DataComponent>(entityId);
		MANI_TEST_ASSERT(componentFromRegistryAfterDelete == nullptr, "Entity's component should be nullptr after delete.");
	}

	MANI_TEST(AddAndRemoveComponents, "Should create a registry and an entity, then add and remove a component")
	{
		struct DataComponent
		{
			int someData = 5;
		};

		// create a registry
		ECS::Registry registry;

		// create an entity
		ECS::EntityId entityId = registry.create();
	
		// add a DataComponent
		DataComponent* dataComponent = registry.add<DataComponent>(entityId);
		MANI_TEST_ASSERT(dataComponent != nullptr, "Component should not be a nullptr.");
		if (dataComponent == nullptr)
		{
			return;
		}

		const bool didRemoveDataComponent = registry.remove<DataComponent>(entityId);
		MANI_TEST_ASSERT(didRemoveDataComponent, "Should have removed the Data component");

		dataComponent = registry.get<DataComponent>(entityId);
		MANI_TEST_ASSERT(dataComponent == nullptr, "Component should be a nullptr.");
	}

	MANI_TEST(RecycleEntities, "Should create a registry and an entity, then properly recycle the entity and its components")
	{
		struct DataComponent
		{
			int someData = 5;
		};

		struct OtherDataComponent {
			int someOtherData = 10;
		};

		// create a registry
		ECS::Registry registry;

		// create an entity
		ECS::EntityId entityId = registry.create();

		// add a DataComponent
		DataComponent* dataComponent = registry.add<DataComponent>(entityId);
		MANI_TEST_ASSERT(dataComponent != nullptr, "Component should not be a nullptr.");
		if (dataComponent == nullptr)
		{
			return;
		}

		// destroy the entity, which should be added to the entity pool
		const bool didDestroy = registry.destroy(entityId);
		MANI_TEST_ASSERT(didDestroy, "Should have detroyed entity");

		dataComponent = registry.get<DataComponent>(entityId);
		MANI_TEST_ASSERT(dataComponent == nullptr, "Component should be a nullptr.");

		// we expect the entity to be recycled
		entityId = registry.create();
		MANI_TEST_ASSERT(entityId == 1, "Should be the first entity addded.");

		// Entity should not have a DataComponent
		dataComponent = registry.get<DataComponent>(entityId);
		MANI_TEST_ASSERT(dataComponent == nullptr, "Component should be a nullptr.");

		OtherDataComponent* otherDataComponent = registry.add<OtherDataComponent>(entityId);
		MANI_TEST_ASSERT(otherDataComponent != nullptr, "Component should not be a nullptr.");
		if (otherDataComponent == nullptr)
		{
			return;
		}

		otherDataComponent->someOtherData += 10;

		OtherDataComponent* otherOtherDataComponent = registry.get<OtherDataComponent>(entityId);
		MANI_TEST_ASSERT(otherOtherDataComponent != nullptr, "Other Component should not be a nullptr.");
		if (otherOtherDataComponent == nullptr)
		{
			return;
		}

		MANI_TEST_ASSERT(otherOtherDataComponent->someOtherData == otherDataComponent->someOtherData, "we should be able to mutate a component of a recycled entity.");
	}

	MANI_TEST(CreateRegistryView, "Should create a registry view and iterate through it")
	{
		struct DataComponent {
			int someData = 5;
		};

		struct OtherDataComponent {
			int someOtherData = 10;
		};

		// create a registry
		ECS::Registry registry;

		// create e few entities, all of them with DataComponent, some of them with OtherDataComponent
		const int entityCount = 10;
		for (int i = 1; i < entityCount; i++)
		{
			const ECS::EntityId entityId = registry.create();
			registry.add<DataComponent>(entityId);
			if (i % 2 == 0)
			{
				registry.add<OtherDataComponent>(entityId);
			}
		}

		MANI_TEST_ASSERT(registry.size() == entityCount, "should have created 10 entities");

		int entityCounter = 1;
		for (ECS::EntityId entityId : ECS::View<DataComponent, OtherDataComponent>(registry))
		{
			entityCounter++;

			auto* component = registry.get<DataComponent>(entityId);
			MANI_TEST_ASSERT(component != nullptr, "should have a DataComponent");

			auto* otherComponent = registry.get<OtherDataComponent>(entityId);
			MANI_TEST_ASSERT(otherComponent != nullptr, "should have a OtherDataComponent");
		}
		MANI_TEST_ASSERT(entityCounter == entityCount / 2, "There should be 5 entities with DataComponent and OtherDataComponent in the registry view");

		// iterate over the entities with DataComponent
		entityCounter = 1;
		for (ECS::EntityId entityId : ECS::View<DataComponent>(registry))
		{
			entityCounter++;
			auto* component = registry.get<DataComponent>(entityId);
			MANI_TEST_ASSERT(component != nullptr, "should have a DataComponent");
		}
		MANI_TEST_ASSERT(entityCounter == entityCount, "There should be 10 entities with DataComponent in the registry view");

		entityCounter = 1;
		for (ECS::EntityId entityId : ECS::View<OtherDataComponent>(registry))
		{
			entityCounter++;
			auto* otherComponent = registry.get<OtherDataComponent>(entityId);
			MANI_TEST_ASSERT(otherComponent != nullptr, "should have a OtherDataComponent");
		}
		MANI_TEST_ASSERT(entityCounter == entityCount / 2, "There should be 5 entities with OtherDataComponent in the registry view");


		entityCounter = 0;
		for (ECS::EntityId entityId : ECS::View<>(registry))
		{
			entityCounter++;
		}
		MANI_TEST_ASSERT(entityCounter == 10, "Should iterate over all entities");
	}

	MANI_TEST(ChaoticEntityCreation, "We should be able to create entities, some without component, and some with components")
	{
		struct DataComponent {
			int someData = 5;
		};

		// create a registry
		ECS::Registry registry;

		// create e few entities, all of them with DataComponent, some of them with OtherDataComponent
		int entityCount = 10;
		// entity ids start at 1 to leave room for the singleton entity
		for (int i = 1; i < entityCount; i++)
		{
			const ECS::EntityId entityId = registry.create();
			if (i % 2 == 0)
			{
				registry.add<DataComponent>(entityId);
			}
		}

		MANI_TEST_ASSERT(registry.size() == entityCount, "should have created 10 entities");

		// iterate over the entities with DataComponent
		int entityCounter = 1;
		for (ECS::EntityId entityId : ECS::View<DataComponent>(registry))
		{
			entityCounter++;
			auto* component = registry.get<DataComponent>(entityId);
			MANI_TEST_ASSERT(component != nullptr, "should have a DataComponent");
		}
		MANI_TEST_ASSERT(entityCounter == entityCount / 2, "There should be 5 entities with DataComponent in the registry view");
	}

	MANI_TEST(ComponentIdAssignation, "ComponentIds should be consecutive")
	{	
		struct Component0 {};
		struct Component1 {};
		struct Component2 {};

		ECS::Registry registry;
		const ECS::ComponentId componentId0 = registry.getComponentId<Component0>();
		const ECS::ComponentId componentId1 = registry.getComponentId<Component1>();
		const ECS::ComponentId componentId2 = registry.getComponentId<Component2>();
		
		MANI_TEST_ASSERT(componentId1 - componentId0 == 1, "The second componentid should follow the first one");
		MANI_TEST_ASSERT(componentId2 - componentId1 == 1, "The third componentid should follow the second one");
	}

	MANI_TEST(Spawn1000000Entities, "Should spawn 1'000'000 entities with a transform")
	{
		struct Transform
		{
			float position[3];
			float rotation[3];
			float scale[3];
		};

		ECS::Registry registry;
		for (int i = 1; i < 1'000'000; ++i)
		{
			const ECS::EntityId entityId = registry.create();
			Transform* transform = registry.add<Transform>(entityId);
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

		MANI_TEST_ASSERT(registry.size() == 1'000'000, "Should have spawned 1'000'000 entities.");
	}

	MANI_TEST(PlayWithDynamicAllocation, "Should allow dynamic allocation")
	{
		struct DynamicComponent
		{
			std::vector<float> vector = { 1.f, 2.f, 3.f };
		};

		ECS::Registry registry;
		const ECS::EntityId entity1 = registry.create();
		const ECS::EntityId entity2 = registry.create();

		DynamicComponent* component1 = registry.add<DynamicComponent>(entity1);
		DynamicComponent* component2 = registry.add<DynamicComponent>(entity2);
		for (int i = 0; i < 1000; ++i)
		{
			component1->vector.push_back(i + 1 * 1000.f);
		}

		MANI_TEST_ASSERT(component2->vector[0] - 1.f <= 1.192092896e-07F, "first vector element should be equal to the original value");
		MANI_TEST_ASSERT(component2->vector[1] - 2.f <= 1.192092896e-07F, "second vector element should be equal to the original value");
		MANI_TEST_ASSERT(component2->vector[2] - 3.f <= 1.192092896e-07F, "third vector element should be equal to the original value");
	}

	MANI_TEST(EntityHasComponent, "Should return true if the entity has the component or not")
	{
		struct Component {};

		ECS::Registry registry;

		const ECS::EntityId entityId = registry.create();

		MANI_TEST_ASSERT(registry.has<Component>(entityId) == false, "Should not have the component yet");

		registry.add<Component>(entityId);

		MANI_TEST_ASSERT(registry.has<Component>(entityId) == true, "Should now have the component");
	}

	MANI_TEST(EntityRegistryEvents, "Should subscribe to all events and confirm that a callback was received")
	{
		struct Component {};

		ECS::Registry registry;

		std::vector<ECS::EntityId> entityIdCreated;
		std::vector<ECS::EntityId> entityIdDestroyed;
		std::vector<std::pair<ECS::EntityId, ECS::ComponentId>> componentIdCreated;
		std::vector<std::pair<ECS::EntityId, ECS::ComponentId>> componentIdDestroyed;

		registry.onEntityCreated.subscribe([&entityIdCreated](ECS::Registry& registry, ECS::EntityId entityId) {
			entityIdCreated.push_back(entityId);
		});

		registry.onEntityDestroyed.subscribe([&entityIdDestroyed](ECS::Registry& registry, ECS::EntityId entityId) {
			entityIdDestroyed.push_back(entityId);
		});

		registry.onComponentAdded.subscribe([&componentIdCreated](ECS::Registry& registry, ECS::EntityId entityId, ECS::ComponentId componentId) {
			componentIdCreated.push_back({ entityId, componentId });
		});

		registry.onComponentRemoved.subscribe([&componentIdDestroyed](ECS::Registry& registry, ECS::EntityId entityId, ECS::ComponentId componentId) {
			componentIdDestroyed.push_back({ entityId, componentId });
		});

		ECS::EntityId entityId = registry.create();
		ECS::ComponentId componentId = registry.getComponentId<Component>();
		registry.add<Component>(entityId);
		registry.remove<Component>(entityId);
		registry.destroy(entityId);

		std::pair<ECS::EntityId, ECS::ComponentId> pair = { entityId, componentId };
		MANI_TEST_ASSERT(entityIdCreated.size() == 1 && entityIdCreated[0] == entityId, "ECS::EntityId should have been created");
		MANI_TEST_ASSERT((componentIdCreated.size() == 1 && componentIdCreated[0] == pair), "ECS::ComponentId should have been added to ECS::EntityId");
		MANI_TEST_ASSERT((componentIdDestroyed.size() == 1 && componentIdDestroyed[0] == pair), "ECS::ComponentId should have been removed from ECS::EntityId");
		MANI_TEST_ASSERT(entityIdDestroyed.size() == 1 && entityIdDestroyed[0] == entityId, "ECS::EntityId should have been destroyed");
	}

	MANI_TEST(BitsetTests, "Should set, test and reset a bitset properly")
	{
		Bitset<64> bitset;
		MANI_TEST_ASSERT(bitset.any() == false, "fresh bitset should not be set");
		bitset.set(0);
		MANI_TEST_ASSERT(bitset.test(0) == true, "bit 0 should have been set");
		MANI_TEST_ASSERT(bitset.test(1) == false, "bit 0 should not have been set");
		bitset.reset();
		MANI_TEST_ASSERT(bitset.any() == false, "fresh bitset should not be set");
		

		bitset.set(5);
		Bitset<64> otherBitset;
		otherBitset.set(5);
		MANI_TEST_ASSERT(bitset == otherBitset, "bitsets should be equal");
		otherBitset.reset();
		MANI_TEST_ASSERT(bitset != otherBitset, "Bitsets should not be equal anymore");

		bitset.reset();
		bitset.set(56);
		MANI_TEST_ASSERT(bitset.test(56) == true, "bit 56 should have been set");

		bitset.reset();
		bitset.set(1);
		bitset.set(5);
		bitset.set(56);

		otherBitset.reset();
		otherBitset.set(1);
		otherBitset.set(5);
		otherBitset.set(56);

		MANI_TEST_ASSERT(bitset == (bitset & otherBitset), "should be able to mask bitsets");

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

		MANI_TEST_ASSERT(resultBitset == (bitset | otherBitset), "should be able to add bitsets");
	}
	
	MANI_TEST(DoNotAssumeRegistrySizeContainsAllIndices, "Should iterate over all the entities")
	{
		struct Component {};

		ECS::Registry registry;

		const ECS::EntityId entity1 = registry.create();
		registry.add<Component>(entity1);
		const ECS::EntityId entity2 = registry.create();
		registry.add<Component>(entity2);
		
		size_t count = 0;
		for (const ECS::EntityId entityId : ECS::View<Component>(registry))
		{
			count++;
		}

		MANI_TEST_ASSERT(count == 2, "All entities should have been visited");

		registry.destroy(entity1);

		count = 0;
		for (const ECS::EntityId entityId : ECS::View<Component>(registry))
		{
			count++;
		}

		MANI_TEST_ASSERT(count == 1, "All entities should have been visited");
	}

	MANI_TEST(CreateEntitiesDuringIteration, "Should be able to create entities during ECS::View iteration without fail")
	{
		struct Component {};
		struct CommonComponent {};
		struct OtherComponent {};

		ECS::Registry registry;

		{
			const ECS::EntityId entityId = registry.create();
			registry.add<Component>(entityId);
			registry.add<CommonComponent>(entityId);
		}

		ECS::View<Component> view(registry);
		for (const ECS::EntityId entityId : view)
		{
			Component* comp = registry.get<Component>(entityId);
			MANI_TEST_ASSERT(comp != nullptr && registry.isValid(entityId), "Entity should be valid");

			const ECS::EntityId newId = registry.create();
			registry.add<CommonComponent>(newId);
			registry.add<OtherComponent>(newId);
		}
	}
	
	MANI_SECTION_BEGIN(TemplatedComponents, "tests on templated components")
	{
		template<typename T>
		struct Component
		{
			T data;
		};

		MANI_TEST(TemplatedComponents, "Should store templated components appropriatly")
		{
			ECS::Registry registry;

			{
				const ECS::EntityId entityId = registry.create();
				auto* component = registry.add<Component<int>>(entityId);
				component->data = 5;

				auto* retrieved = registry.get<Component<int>>(entityId);
				MANI_TEST_ASSERT(retrieved != nullptr && retrieved->data == 5, "Should create and retrieve templated component");
			}
		}
	}
	MANI_SECTION_END(TemplatedComponents)
}
MANI_SECTION_END(ECS)