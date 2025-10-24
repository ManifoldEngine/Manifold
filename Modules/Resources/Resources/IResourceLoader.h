#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	class IResourceLoader
	{
	public:
		// The component id of the resource that this loader handles.
		// the expexted id is the templated resource id as this is what is 
		// held by the registry when a resource is loaded.
		// The component id of a resource can be retrieved as follow:
		//	example: registry.getComponentId<Mani::Resource<Texture>>();
		virtual ECS::ComponentId getComponentId(const ECS::Registry& registry) const = 0;

		// this function is called whenever the ResourceSystem is trying 
		// to load a resource with the same component id as is returned 
		// by this interface.
		virtual bool load(ECS::Registry& registry, const std::filesystem::path& absolutePath, ECS::EntityId resourceId, uint32_t tag) const = 0;
	};
}