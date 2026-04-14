#include <ManiTests/ManiTests.h>

#include <Core/ManiTests.h>
#include <Core/CoreFwd.h>

#include <Resources/Resources.h>
#include <Resources/ResourceSystem.h>

#ifndef MANI_WEBGL
extern "C" __declspec(dllexport) void runTests()
{
	Mani::runTests();
}
#endif

using namespace Mani;

struct TestResource
{
	int value = 0;
};

namespace Mani::DefaultResourceLoader
{
	template<>
	bool load<TestResource>(ECS::Registry& registry, const Path& absolutePath, Resource<TestResource>& resource, uint32_t)
	{
		// Simulate loading
		resource.value.value = 42;
		return true;
	}
}

MANI_SECTION_BEGIN(ResourcesTests, "Resources")
{
	MANI_TEST(ShouldLoadSync, "Should load resource synchronously correctly")
	{
		Application app;
		World& world = app.getWorld();
		ECS::Registry& registry = world.getMutableRegistry();
		world.createSystem<ResourceSystem>();

		auto entityId = Resources::loadSync<TestResource>(registry, "dummy.json");

		MANI_TEST_ASSERT(registry.isValid(entityId), "Entity should exist");
		MANI_TEST_ASSERT(Resources::isReady(registry, entityId), "Resource should be ready immediately");

		auto& res = registry.getPinned<Resource<TestResource>>(entityId);
		MANI_TEST_ASSERT(res.value.value == 42, "Loaded value mismatch");
	}

	MANI_TEST(ShouldLoadSyncAlreadyLoadedPath, "Should return the entityId of a resource that's already loaded")
	{
		Application app;
		World& world = app.getWorld();
		ECS::Registry& registry = world.getMutableRegistry();
		world.createSystem<ResourceSystem>();

		{
			auto entityId = Resources::loadSync<TestResource>(registry, "dummy.json");

			MANI_TEST_ASSERT(registry.isValid(entityId), "Entity should exist");
			MANI_TEST_ASSERT(Resources::isReady(registry, entityId), "Resource should be ready immediately");

			auto& res = registry.getPinned<Resource<TestResource>>(entityId);
			MANI_TEST_ASSERT(res.value.value == 42, "Loaded value mismatch");
		}

		{
			const auto entityId = Resources::load<TestResource>(registry, "dummy.json");
			MANI_TEST_ASSERT(Resources::isReady(registry, entityId), "Resource should be already loaded");
		}
	}

	MANI_TEST(ShouldLoadAsync, "Should load resource asynchronously correctly")
	{
		Application app;
		World& world = app.getWorld();
		ECS::Registry& registry = world.getMutableRegistry();
		world.createSystem<ResourceSystem>();

		auto entityId = Resources::load<TestResource>(registry, "dummy.json");

		MANI_TEST_ASSERT(registry.isValid(entityId), "Entity should exist");
		MANI_TEST_ASSERT(!Resources::isReady(registry, entityId), "Should not be ready immediately");

		// Pump the world until ready
		for (int i = 0; i < 100 && !Resources::isReady(registry, entityId); ++i)
		{
			app.tick();
		}

		MANI_TEST_ASSERT(Resources::isReady(registry, entityId), "Resource should eventually be ready");

		auto& res = registry.getPinned<Resource<TestResource>>(entityId);
		MANI_TEST_ASSERT(res.value.value == 42, "Loaded value mismatch");
	}

	MANI_TEST(ShouldInject, "Should inject resource correctly")
	{
		Application app;
		World& world = app.getWorld();
		ECS::Registry& registry = world.getMutableRegistry();
		world.createSystem<ResourceSystem>();

		TestResource data;
		data.value = 1337;

		auto entityId = Resources::inject<TestResource>(registry, std::move(data));

		MANI_TEST_ASSERT(registry.isValid(entityId), "Entity should exist");
		MANI_TEST_ASSERT(Resources::isReady(registry, entityId), "Injected resource should be ready");

		auto& res = registry.getPinned<Resource<TestResource>>(entityId);
		MANI_TEST_ASSERT(res.value.value == 1337, "Injected value mismatch");
	}

	MANI_TEST(ShouldUnloadSingle, "Should unload a specific resource")
	{
		Application app;
		World& world = app.getWorld();
		ECS::Registry& registry = world.getMutableRegistry();
		world.createSystem<ResourceSystem>();

		auto entityId = Resources::loadSync<TestResource>(registry, "dummy.json");

		Resources::unload(registry, entityId);

		app.tick(); // Allow one tick as resource destruction is handled at the end of the frame

		MANI_TEST_ASSERT(!registry.isValid(entityId), "Entity should be destroyed after unload");
	}

	MANI_TEST(ShouldUnloadTag, "Should unload resources by tag")
	{
		Application app;
		World& world = app.getWorld();
		ECS::Registry& registry = world.getMutableRegistry();
		world.createSystem<ResourceSystem>();

		uint32_t tagA = 1;
		uint32_t tagB = 2;

		auto e1 = Resources::loadSync<TestResource>(registry, "a.json", tagA);
		auto e2 = Resources::loadSync<TestResource>(registry, "b.json", tagB);

		Resources::unloadTag(registry, tagA);

		app.tick(); // Allow one tick as resource destruction is handled at the end of the frame

		MANI_TEST_ASSERT(!registry.isValid(e1), "Tag A resource should be removed");
		MANI_TEST_ASSERT(registry.isValid(e2), "Tag B resource should remain");
	}

	MANI_TEST(ShouldUnloadAll, "Should unload all resources")
	{
		Application app;
		World& world = app.getWorld();
		ECS::Registry& registry = world.getMutableRegistry();
		world.createSystem<ResourceSystem>();

		auto e1 = Resources::loadSync<TestResource>(registry, "a.json");
		auto e2 = Resources::loadSync<TestResource>(registry, "b.json");

		Resources::unloadAll(registry);

		app.tick(); // Allow one tick as resource destruction is handled at the end of the frame

		MANI_TEST_ASSERT(!registry.isValid(e1), "All resources should be removed");
		MANI_TEST_ASSERT(!registry.isValid(e2), "All resources should be removed");
	}

	MANI_TEST(AsyncEventuallyReady, "Async resource should not stay pending forever")
	{
		Application app;
		World& world = app.getWorld();
		ECS::Registry& registry = world.getMutableRegistry();
		world.createSystem<ResourceSystem>();

		auto entityId = Resources::load<TestResource>(registry, "dummy.json");

		bool isReady = false;

		for (int i = 0; i < 200; ++i)
		{
			app.tick();
			if (Resources::isReady(registry, entityId))
			{
				isReady = true;
				break;
			}
		}

		MANI_TEST_ASSERT(isReady, "Async resource never became ready");
	}

	MANI_TEST(InjectDoesNotDependOnUpdate, "Injected resources should not require world update")
	{
		Application app;
		World& world = app.getWorld();
		ECS::Registry& registry = world.getMutableRegistry();
		world.createSystem<ResourceSystem>();

		auto entityId = Resources::inject<TestResource>(registry, TestResource{ 55 });

		MANI_TEST_ASSERT(Resources::isReady(registry, entityId), "Should be ready immediately without update");

		auto& res = registry.getPinned<Resource<TestResource>>(entityId);
		MANI_TEST_ASSERT(res.value.value == 55, "Injected value mismatch");
	}

	MANI_SECTION_BEGIN(ResourcesDependency, "Resource Dependency flow")
	{
		struct TestResourceAExt
		{
			int value = 0;
		};

		struct TestResourceA
		{
		};

		struct DependencyExtension final : IResourceSystemExtension
		{
			void onResourceLoaded(ECS::Registry& registry, ECS::EntityId entityId, uint32_t) const override
			{
				if (!registry.hasPinned<Resource<TestResourceA>>(entityId))
				{
					return;
				}

				MANI_TEST_ASSERT(!Resources::isReady(registry, entityId), "resource A is not ready.");

				auto& resA = registry.getPinned<Resource<TestResourceA>>(entityId);
				auto& ext = registry.addPinned<TestResourceAExt>(entityId);
				ext.value = 7;
			}

			void onResourceUnloaded(ECS::Registry&, ECS::EntityId, uint32_t) const override {}
		};

		struct TestResourceALoader final : public IResourceLoader
		{
			bool load(ECS::Registry& registry, const Path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const override
			{
				return true;
			}
		};

		MANI_TEST(ShouldLoadWithDependency, "Resource A should load and trigger its extension")
		{
			Application app;
			World& world = app.getWorld();
			ECS::Registry& registry = world.getMutableRegistry();
			world.createSystem<ResourceSystem>();

			DependencyExtension extension;
			TestResourceALoader loaderA;
			Resources::registerExtension(registry, &extension);
			Resources::registerLoaderFor<TestResourceA>(registry, &loaderA);

			auto entityIdA = Resources::load<TestResourceA>(registry, "resource_a.json");

			bool isAReady = false;

			for (int i = 0; i < 200; ++i)
			{
				app.tick();

				isAReady = Resources::isReady(registry, entityIdA);
				if (isAReady)
				{
					break;
				}
			}

			MANI_TEST_ASSERT(isAReady, "Resource A should be ready");
			
			auto& resExt = registry.getPinned<TestResourceAExt>(entityIdA);
			MANI_TEST_ASSERT(resExt.value == 7, "Dependency B value mismatch");

			Resources::unregisterLoaderFor<TestResourceA>(registry);
			Resources::unregisterExtension(registry, &extension);
		}

		struct TestResourceB
		{
			int value = 0;
		};

		struct TestResourceBLoader final : public IResourceLoader
		{
			bool load(ECS::Registry& registry, const Path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const 
			{ 
				auto& res = registry.getPinned<Resource<TestResourceB>>(resourceId);
				res.value.value = 420;
				return true;
			};
		};
	
		struct TestResourceC
		{
			std::string resourceAPath = "dummyA.json";
			std::string resourceBPath = "dummyB.json";

			ECS::EntityId resourceAId = ECS::INVALID_ID;
			ECS::EntityId resourceBId = ECS::INVALID_ID;
		};

		struct TestResourceCLoader final : public IResourceLoader
		{
			bool load(ECS::Registry& registry, const Path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const { return true; };
			void postLoad(ECS::Registry& registry, const Path& absolutePath, ECS::EntityId resourceId, EResourceLoadMethod method, uint32_t tag) const
			{
				auto& res = registry.getPinned<Resource<TestResourceC>>(resourceId);
				res.value.resourceAId = Resources::load<TestResourceA>(registry, res.value.resourceAPath, method, tag);
				res.value.resourceBId = Resources::load<TestResourceB>(registry, res.value.resourceBPath, method, tag);
			};
		};

		MANI_TEST(PostLoadFlow, "Resource C should load A and B in its post load phase")
		{
			Application app;
			World& world = app.getWorld();
			ECS::Registry& registry = world.getMutableRegistry();
			world.createSystem<ResourceSystem>();

			DependencyExtension extension;
			TestResourceALoader loaderA;
			TestResourceBLoader loaderB;
			TestResourceCLoader loaderC;
			Resources::registerExtension(registry, &extension);
			Resources::registerLoaderFor<TestResourceA>(registry, &loaderA);
			Resources::registerLoaderFor<TestResourceB>(registry, &loaderB);
			Resources::registerLoaderFor<TestResourceC>(registry, &loaderC);

			auto entityIdC = Resources::load<TestResourceC>(registry, "resource_c.json");

			bool isAReady = false;
			bool isBReady = false;
			bool isCReady = false;

			for (int i = 0; i < 200; ++i)
			{
				app.tick();

				if (!isCReady)
				{
					isCReady = Resources::isReady(registry, entityIdC);
					continue;
				}

				auto& resC = registry.getPinned<Resource<TestResourceC>>(entityIdC);

				isAReady = Resources::isReady(registry, resC.value.resourceAId);
				isBReady = Resources::isReady(registry, resC.value.resourceBId);

				if (isAReady && isBReady)
				{
					break;
				}
			}

			MANI_TEST_ASSERT(isAReady, "Resource A should be ready");
			MANI_TEST_ASSERT(isBReady, "Resource B should be ready");
			MANI_TEST_ASSERT(isCReady, "Resource B should be ready");

			auto& resC = registry.getPinned<Resource<TestResourceC>>(entityIdC);
			MANI_TEST_ASSERT(resC.value.resourceAId != ECS::INVALID_ID, "Dependency C value mismatch");
			MANI_TEST_ASSERT(resC.value.resourceBId != ECS::INVALID_ID, "Dependency C value mismatch");

			MANI_TEST_ASSERT(registry.isValid(resC.value.resourceAId) && registry.hasPinned<Resource<TestResourceA>>(resC.value.resourceAId), "A should be properly loaded");
			auto& resB = registry.getPinned<Resource<TestResourceB>>(resC.value.resourceBId);
			MANI_TEST_ASSERT(resB.value.value == 420, "B should be properly loaded");

			Resources::unregisterExtension(registry, &extension);
			Resources::unregisterLoaderFor<TestResourceA>(registry);
			Resources::unregisterLoaderFor<TestResourceB>(registry);
		}
	}
	MANI_SECTION_END(ResourcesDependency)
}
MANI_SECTION_END(ResourcesTests)