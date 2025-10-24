#pragma once

#include <Core/CoreFwd.h>
#include <Core/FileSystem.h>
#include <Core/Async/Parallel.h>

#include <Resources/Components/Resource.h>
#include <Resources/Components/ResourceStorage.h>

#include <Resources/IResourceLoader.h>
#include <Resources/IResourceSystemExtension.h>

#include <ManiZ/ManiZ.h>
#include <string_view>
#include <limits>

namespace Mani
{
    constexpr std::string_view LogResources = "Resources";
    constexpr uint32_t GLOBAL_RESOURCE_TAG = (std::numeric_limits<uint32_t>::max)();

	namespace DefaultResourceLoader
	{
		// Loads a JSON file into a resource.
		// Specialize this function to define loading behavior for specific resource types.
		// Returns true if the resource was successfully loaded.
		template<typename T>
		static bool load(ECS::Registry& registry, const std::filesystem::path& absolutePath, Resource<T>& resource, uint32_t tag);
	}

	// The Resources class provides utility functions for managing resource loading and unloading.
	// 
	// When a resource is requested:
	//   - A new entity is created.
	//   - A Resource<T> component is attached to that entity.
	//   - A loading task is queued (asynchronously by default).
	//
	// Once the loading task completes, the Resource<T>::isReady flag is set to true.
	//
	// Resources can be tagged to group them logically, making it easier to unload them when the
	// application's context changes (e.g., switching levels or scenes).
	//
	// This is implemented as a class (rather than a namespace) to allow private helper functions.
	class Resources
	{
	public:
		// Asynchronously loads a resource of type T from the given relative path.
		// Returns the entity id representing the resource.
		template<typename T>
		static ECS::EntityId load(ECS::Registry& registry, const std::filesystem::path& relativePath, uint32_t = 0);
		
		// Synchronously loads a resource of type T from the given relative path.
		// Returns the entity id representing the resource.
		template<typename T>
		static ECS::EntityId loadSync(ECS::Registry& registry, const std::filesystem::path& relativePath, uint32_t = 0);

		// Unloads a specific resource associated with the given entity ID.
		static void unload(ECS::Registry& registry, ECS::EntityId inEntityId);

		// Unloads all loaded resources.
		static void unloadAll(ECS::Registry& registry);

		// Unloads all resources that share the specified tag.
		static void unloadTag(ECS::Registry& registry, uint32_t tag);

		// Injects an already constructed resource value into the registry.
		// Returns the entity id representing the injected resource.
		template<typename T>
		static ECS::EntityId inject(ECS::Registry& registry, T&& value, uint32_t = 0);

		// Registers a system extension to be notified of resource-related events.
		static void registerExtension(ECS::Registry& registry, IResourceSystemExtension* extension);

		// Unregisters a previously registered system extension.
		static void unregisterExtension(ECS::Registry& registry, IResourceSystemExtension* extension);

		// Registers a custom resource loader for handling specific file types or formats.
		static void registerLoader(ECS::Registry& registry, IResourceLoader* loader);
		
		// Unregisters a previously registered resource loader.
		static void unregisterLoader(ECS::Registry& registry, IResourceLoader* loader);

	private:
		enum class ELoadMethod : uint8_t
		{
			Async = 0,
			Sync,
		};

		template<typename T>
		static ECS::EntityId load_impl(ECS::Registry& registry, const std::filesystem::path& relativePath, uint32_t tag, ELoadMethod method);

		template<typename T>
		static std::tuple<ECS::EntityId, Resource<T>&> createResource(ECS::Registry& registry, uint32_t tag);

		static void forEachExtension(const ECS::Registry& registry, auto&& f);
	};

	template<typename T>
	ECS::EntityId Resources::load(ECS::Registry& registry, const std::filesystem::path& relativePath, uint32_t tag)
	{
		return Resources::load_impl<T>(registry, relativePath, tag, ELoadMethod::Async);
	}

	template<typename T>
	static ECS::EntityId Resources::loadSync(ECS::Registry& registry, const std::filesystem::path& relativePath, uint32_t tag)
	{
		return Resources::load_impl<T>(registry, relativePath, tag, ELoadMethod::Sync);
	}

	template<typename T>
	ECS::EntityId Resources::load_impl(ECS::Registry& registry, const std::filesystem::path& relativePath, uint32_t tag, ELoadMethod method)
	{
		const auto path = FileSystem::getAbsolutePath(relativePath);

		ResourceStorage& storage = *registry.getSingle<ResourceStorage>();
		{
			std::lock_guard<std::mutex> lock(storage.pathToEntityMutex);

			if (auto* entityId = storage.pathToEntityId.find(path))
			{
				return *entityId;
			}
		}

		auto [entityId, resource] = createResource<T>(registry, tag);
		{
			std::lock_guard<std::mutex> lock(storage.pathToEntityMutex);
			MANI_LOG(LogResources, "Loading asset at {}...", path.string());
			storage.pathToEntityId[path] = entityId;
		}

		auto load = [&, entityId, path, tag, method]
		{
			const IResourceLoader* loader = nullptr;
			{
				std::lock_guard<std::mutex> lock(storage.resourceLoaderMutex);
				const ECS::ComponentId componentId = registry.getComponentId<Resource<T>>();
				if (auto** loaderPtr = storage.loaders.find(componentId))
				{
					loader = *loaderPtr;
				}
			}

			bool wasLoaded = false;
			if (loader != nullptr)
			{
				wasLoaded = loader->load(registry, path, entityId, tag);
			}
			else
			{
				wasLoaded = DefaultResourceLoader::load<T>(registry, path, resource, tag);
			}

			if (!wasLoaded)
			{
				MANI_LOG_ERROR(LogResources, "Could not load resource at {}", path.string());
				return;
			}

			resource.isReady = true;

			switch (method)
			{
			case ELoadMethod::Sync:
			{
				forEachExtension(registry, [&registry, entityId, tag](const IResourceSystemExtension& ext)
				{
					ext.onResourceLoaded(registry, entityId, tag);
				});
				break;
			}
			case ELoadMethod::Async:
			{
				// the extension promises that it will be called on the main thread so we defer it to the end of the frame.
				Mani::defer([&registry, entityId, tag] {
					forEachExtension(registry, [&registry, entityId, tag](const IResourceSystemExtension& ext)
					{
						ext.onResourceLoaded(registry, entityId, tag);
					});
				});
				break;
			}
			}
		};

		switch (method)
		{
		case ELoadMethod::Async:
			Mani::enqueueTask(load);
			break;
		case ELoadMethod::Sync:
			load();
			break;
		}

		return entityId;
	}

	template<typename T>
	ECS::EntityId Resources::inject(ECS::Registry& registry, T&& value, uint32_t tag)
	{
		auto [entityId, resource] = createResource<T>(registry, tag);
		resource.value = std::move(value);
		resource.isReady = true;
		Resources::forEachExtension(registry, [&registry, &entityId, tag](const IResourceSystemExtension& ext)
		{
			ext.onResourceLoaded(registry, entityId, tag);
		});
		return entityId;
	}

	template<typename T>
	std::tuple<ECS::EntityId, Resource<T>&> Resources::createResource(ECS::Registry& registry, uint32_t tag)
	{
		ECS::EntityId entityId = registry.create();
		auto [resourceTag, resource] = registry.addMany<ResourceTag, Resource<T>>(entityId);
		resourceTag->tag = static_cast<uint32_t>(tag);
		return { entityId, *resource };
	}

	void Resources::forEachExtension(const ECS::Registry& registry, auto&& f)
	{
		auto* storage = registry.getSingle<ResourceStorage>();
		MANI_ASSERT(storage != nullptr, "outside of the lifetime of resource system");
		for (const auto* extension : storage->extensions)
		{
			f(*extension);
		}
	}

	template<typename T>
	bool DefaultResourceLoader::load(ECS::Registry& registry, const std::filesystem::path& absolutePath, Resource<T>& resource, uint32_t tag)
	{
		std::string content;
		if (!FileSystem::readFile(absolutePath, content))
		{
			MANI_LOG_ERROR(LogResources, "Could not find asset at path {}", absolutePath.string());
			return false;
		}

		resource.value = ManiZ::from::json<T>(content);
		return true;
	}
}
