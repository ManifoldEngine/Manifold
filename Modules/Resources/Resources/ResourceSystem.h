#pragma once

#include <Core/CoreFwd.h>
#include <Core/Async/Parallel.h>
#include <ManiMaths/Traits.h>

#include <Resources/Resources.h>
#include <Resources/Resource.h>
#include <Resources/IResourceSystemExtension.h>

#include <Core/FileSystem.h>

#include <ManiZ/ManiZ.h>

namespace Mani
{
	class IResourceSystemExtension;

	namespace ResourceLoader
	{
		// loads a json file into a resource. Specialize this function to define loading for specific resources.
		// returns true if loading was successful
		template<typename T>
		static bool load(const std::filesystem::path& absolutePath, Resource<T>& resource);
	}

	class ResourceSystem : public ECS::System
	{
	private:
		struct Storage
		{
			std::unordered_map<std::filesystem::path, ECS::EntityId> pathToEntityId;
			std::mutex pathToEntityMutex;
		};

	public:
		virtual std::string_view getName() const override { return "ResourceSystem"; }
		virtual bool shouldTick(ECS::Registry& registry) const override { return false; }

		template<typename T>
		static ECS::EntityId loadResource(ECS::Registry& registry, const std::filesystem::path& relativePath, uint32_t = 0);

		template<typename T>
		static ECS::EntityId loadResourceSync(ECS::Registry& registry, const std::filesystem::path& relativePath, uint32_t = 0);
		
		static void unloadResource(ECS::Registry& registry, ECS::EntityId inEntityId);
		static void unloadAll(ECS::Registry& registry);
		static void unloadTag(ECS::Registry& registry, uint32_t tag);

		template<typename T>
		static ECS::EntityId injectResource(ECS::Registry& registry, T&& value, uint32_t = 0);

		static ECS::EntityId addExtension(ECS::Registry& registry, std::unique_ptr<IResourceSystemExtension> extension);
		static void removeExtension(ECS::Registry& registry, ECS::EntityId);

	protected:
		virtual void onInitialize(ECS::Registry& registry, World& world) override;
		virtual void onDeinitialize(ECS::Registry& registry) override;

	private:
		enum class ELoadMethod : uint8_t
		{
			Async = 0,
			Sync,
		};

		template<typename T>
		static ECS::EntityId loadResource(ECS::Registry& registry, const std::filesystem::path& relativePath, uint32_t tag, ELoadMethod method);

		template<typename T>
		static std::tuple<ECS::EntityId, Resource<T>&> createResource(ECS::Registry& registry, uint32_t tag);
		
		static void forEachExtension(const ECS::Registry& registry, auto&& f);
	};

	template<typename T>
	ECS::EntityId ResourceSystem::loadResource(ECS::Registry& registry, const std::filesystem::path& relativePath, uint32_t tag)
	{
		return loadResource<T>(registry, relativePath, tag, ELoadMethod::Async);
	}

	template<typename T>
	static ECS::EntityId ResourceSystem::loadResourceSync(ECS::Registry& registry, const std::filesystem::path& relativePath, uint32_t tag)
	{
		return loadResource<T>(registry, relativePath, tag, ELoadMethod::Sync);
	}

	template<typename T>
	ECS::EntityId ResourceSystem::loadResource(ECS::Registry& registry, const std::filesystem::path& relativePath, uint32_t tag, ELoadMethod method)
	{
		const auto path = FileSystem::getAbsolutePath(relativePath);

		ResourceSystem::Storage& storage = *registry.getSingle<ResourceSystem::Storage>();
		{
			std::lock_guard<std::mutex> lock(storage.pathToEntityMutex);

			auto it = storage.pathToEntityId.find(path);
			if (it != storage.pathToEntityId.end())
			{
				return it->second;
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
			if (!ResourceLoader::load(path, resource))
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
	ECS::EntityId ResourceSystem::injectResource(ECS::Registry& registry, T&& value, uint32_t tag)
	{
		auto [entityId, resource] = createResource<T>(registry, tag);
		resource.value = std::move(value);
		resource.isReady = true;
		forEachExtension(registry, [&registry, &entityId, tag](const IResourceSystemExtension& ext)
		{
			ext.onResourceLoaded(registry, entityId, tag);
		});
		return entityId;
	}

	template<typename T>
	std::tuple<ECS::EntityId, Resource<T>&> ResourceSystem::createResource(ECS::Registry& registry, uint32_t tag)
	{
		ECS::EntityId entityId = registry.create();
		auto [resourceTag, resource] = registry.addMany<ResourceTag, Resource<T>>(entityId);
		resourceTag->tag = static_cast<uint32_t>(tag);
		return { entityId, *resource };
	}

	void ResourceSystem::forEachExtension(const ECS::Registry& registry, auto&& f)
	{
		for (const auto entityId : ECS::View<ResourceSystemExtension>(registry))
		{
			const ResourceSystemExtension& ext = *registry.get<ResourceSystemExtension>(entityId);
			f(*ext.obj);
		}
	}

	template<typename T>
	bool ResourceLoader::load(const std::filesystem::path& absolutePath, Resource<T>& resource)
	{
		std::string content;
		if (!FileSystem::readFile(absolutePath, content))
		{
			MANI_LOG_ERROR(LogResources, "Could not find asset at path {}", absolutePath.string());
			return false;
		}

		resource.value = ManiZ::from::json<T>(std::move(content));
		return true;
	}
}
