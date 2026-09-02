#pragma once

#include <Core/CoreFwd.h>
#include <Resources/Components/Resource.h>

namespace Mani
{
	// Interface for defining custom resource loaders.
	// 
	// Each loader is responsible for handling a specific resource type,
	// typically identified by its component ID. When the ResourceSystem
	// attempts to load a resource, it looks up the appropriate loader based
	// on that ID and delegates the loading operation.
	class IResourceLoader
	{
	public:
		// Called when loading a resource, might be called on a task thread
		//
		// Called automatically by the ResourceSystem when a resource with a
		// matching component ID needs to be loaded.
		//
		// Parameters:
		//   registry: The ECS registry that owns the resource entity.
		//   absolutePath: The absolute filesystem path to the resource file.
		//   resourceId: The entity ID representing the resource. Access with registry.get<Resource<T>>(resourceId)
		//   tag: An optional tag used to group or categorize resources.
		//
		// Returns true if the resource was successfully loaded.
		virtual bool load(ECS::Registry& registry, const Path& absolutePath, EntityId resourceId, uint32_t tag) const = 0;

		// Called after loading, is guaranteed to be called on the registry's thread 
		// use this to do any main thread syncing like adding components or creating entities
		// 
		// Parameters:
		//   registry: The ECS registry that owns the resource entity.
		//   absolutePath: The absolute filesystem path to the resource file.
		//   resourceId: The entity ID representing the resource. Access with registry.get<Resource<T>>(resourceId)
		//   tag: An optional tag used to group or categorize resources.
		virtual void postLoad(ECS::Registry& registry, const Path& absolutePath, EntityId resourceId, EResourceLoadMethod method, uint32_t tag) const {};

		// Called when unloading a resource, might be called on a task thread
		//
		// Called automatically by the ResourceSystem when a resource with a
		// matching component ID needs to be unloaded.
		//
		// Parameters:
		//   registry: The ECS registry that owns the resource entity.
		//   resourceId: The entity ID representing the resource. Access with registry.get<Resource<T>>(resourceId)
		//
		// Returns true if the resource was successfully loaded.
		virtual bool unload(ECS::Registry& registry, EntityId resourceId) const = 0;
	};
}