#pragma once

#include <Core/CoreFwd.h>

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
		// Returns the component ID of the resource type handled by this loader.
		//
		// The expected ID corresponds to the templated resource component type
		// (e.g. Resource<Texture>) registered in the ECS registry.
		//
		// Example:
		//     registry.getComponentId<Mani::Resource<Texture>>();
		virtual ECS::ComponentId getComponentId(const ECS::Registry& registry) const = 0;

		// Loads a resource instance into the ECS registry.
		//
		// Called automatically by the ResourceSystem when a resource with a
		// matching component ID needs to be loaded.
		//
		// Parameters:
		//   - registry: The ECS registry that owns the resource entity.
		//   - absolutePath: The absolute filesystem path to the resource file.
		//   - resourceId: The entity ID representing the resource.
		//		- accessed with registry.getRef<Resource<T>>(resourceId)
		//   - tag: An optional tag used to group or categorize resources.
		//
		// Returns true if the resource was successfully loaded.
		virtual bool load(ECS::Registry& registry, const std::filesystem::path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const = 0;
	};
}