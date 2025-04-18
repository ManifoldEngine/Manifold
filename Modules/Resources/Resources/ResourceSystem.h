#pragma once

#include <Core/CoreFwd.h>
#include <Core/Thread/Parallel.h>

#include <Resources/Resources.h>
#include <Resources/Resource.h>
#include <Resources/IResourceSystemExtension.h>

#include <Core/ManiTraits.h>
#include <Core/FileSystem.h>

#include <ManiZ/ManiZ.h>

namespace Mani
{
	class IResourceSystemExtension;

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
		static ECS::EntityId loadResource(ECS::Registry& registry, const std::filesystem::path& relativePath);

		template<typename T>
		static ECS::EntityId loadResourceSync(ECS::Registry& registry, const std::filesystem::path& relativePath);
		
		template<typename T>
		static void unloadResource(ECS::Registry& registry, ECS::EntityId inEntityId);

		template<typename T>
		static ECS::EntityId injectResource(ECS::Registry& registry, std::unique_ptr<T> value);

		static ECS::EntityId addExtension(ECS::Registry& registry, std::shared_ptr<IResourceSystemExtension> extension);
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
		static ECS::EntityId loadResource(ECS::Registry& registry, const std::filesystem::path& relativePath, ELoadMethod method);

		template<typename T>
		static std::tuple<ECS::EntityId, Resource<T>&> createResource(ECS::Registry& registry);
		
		static void forEachExtension(const ECS::Registry& registry, auto&& f);
	};

	template<typename T>
	ECS::EntityId ResourceSystem::loadResource(ECS::Registry& registry, const std::filesystem::path& relativePath)
	{
		return loadResource<T>(registry, relativePath, ELoadMethod::Async);
	}

	template<typename T>
	static ECS::EntityId ResourceSystem::loadResourceSync(ECS::Registry& registry, const std::filesystem::path& relativePath)
	{
		return loadResource<T>(registry, relativePath, ELoadMethod::Sync);
	}

	template<typename T>
	ECS::EntityId ResourceSystem::loadResource(ECS::Registry& registry, const std::filesystem::path& relativePath, ELoadMethod method)
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

		auto [entityId, resource] = createResource<T>(registry);
		{
			std::lock_guard<std::mutex> lock(storage.pathToEntityMutex);
			MANI_LOG(LogResources, "Loading asset at {}...", path.string());
			storage.pathToEntityId[path] = entityId;
		}

		auto load = [entityId, path, &resource]
		{
			std::string content;
			if (!FileSystem::readFile(path, content))
			{
				MANI_LOG_ERROR(LogResources, "Could not find asset at path {}", path.string());
				return;
			}

			resource.value = std::make_unique<T>(ManiZ::from::json<T>(content));
			resource.isReady = true;
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
		
		forEachExtension(registry, [&registry, &entityId](const IResourceSystemExtension& ext)
		{
			ext.onResourceCreated(registry, entityId);
		});
		return entityId;
	}

	template<typename T>
	ECS::EntityId ResourceSystem::injectResource(ECS::Registry& registry, std::unique_ptr<T> value)
	{
		auto [entityId, resource] = createResource<T>(registry);
		resource.value = std::move(value);
		resource.isReady = true;
		forEachExtension(registry, [&registry, &entityId](const IResourceSystemExtension& ext)
		{
			ext.onResourceCreated(registry, entityId);
		});
		return entityId;
	}

	template<typename T>
	void ResourceSystem::unloadResource(ECS::Registry& registry, ECS::EntityId inEntityId)
	{
		forEachExtension(registry, [&registry, inEntityId](const IResourceSystemExtension& ext)
		{
			ext.onResourceDestroyed(registry, inEntityId);
		});

		ResourceSystem::Storage& storage = *registry.getSingle<ResourceSystem::Storage>();
		registry.destroy(inEntityId);

		{
			std::lock_guard<std::mutex> lock(storage.pathToEntityMutex);
			for (const auto [path, entityId] : storage.pathToEntityId)
			{
				if (entityId == inEntityId)
				{
					MANI_LOG(LogResources, "Unloading asset at {}", path.string());
					storage.pathToEntityId.erase(path);
					return;
				}
			}
		}
	}

	template<typename T>
	std::tuple<ECS::EntityId, Resource<T>&> ResourceSystem::createResource(ECS::Registry& registry)
	{
		ECS::EntityId entityId = registry.create();
		Resource<T>& resource = *registry.add<Resource<T>>(entityId);
		return { entityId, resource };
	}

	void ResourceSystem::forEachExtension(const ECS::Registry& registry, auto&& f)
	{
		for (const auto entityId : ECS::View<ResourceSystemExtension>(registry))
		{
			const ResourceSystemExtension& ext = *registry.get<ResourceSystemExtension>(entityId);
			f(*ext.obj);
		}
	}
}