#pragma once

#include "ECS.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <map>
#include <stdexcept>
#include <OSUtils/OSUtils.h>
#include "Entity.h"

// TODO: do not use STL for exposed classes.
// warning C4251: 'ECSEngine::EntityRegistry::ComponentStore': class 'std::vector<unsigned char,std::allocator<unsigned char>>' needs to have dll-interface to be used by clients of class 'ECSEngine::EntityRegistry'
#pragma warning(disable:4251)

namespace ECSEngine {

	class ECS_API EntityRegistry
	{
	
	public:
		struct ECS_API QueryResult {
			uint32_t Count = 0;
			std::vector<EntityId> EntityIds;
		};

		uint32_t Create();
		void Destroy(EntityId entityId);
		
		template<typename TComponent>
		bool AddComponent(EntityId entityId, const TComponent& component);
		
		template<typename TComponent>
		bool RemoveComponent(EntityId entityId);

		template<typename TComponent>
		bool HasComponent(EntityId entityId);

		//template<class ... TComponents>
		//bool Query(QueryResult& OutResult, const TComponents& ... args);

	private:
		std::vector<unsigned char> ComponentStore;
		std::vector<Entity> Entities;
		// std::map<std::string_view, ComponentId> TypeNameToTypeIdMap;

		bool IsValid(EntityId entityId) const;
		
		bool AddComponent_Internal(EntityId entityId, ComponentId componentId);
		bool RemoveComponent_Internal(EntityId entityId, ComponentId componentId);
		bool HasComponent_Internal(EntityId entityId, ComponentId componentId);
		//bool Query_Internal(QueryResult& OutResult, const std::vector<ComponentId>& componentIds);

		// Converts a TComponent type into a numerical identifier.
		template<typename TComponent>
		ComponentId GetComponentId() const;
	};

	template<typename TComponent>
	inline bool EntityRegistry::AddComponent(EntityId entityId, const TComponent& component)
	{
		ComponentId typeId = GetComponentId<TComponent>();
		return AddComponent_Internal(entityId, typeId);
	}

	template<typename TComponent>
	inline bool EntityRegistry::RemoveComponent(EntityId entityId)
	{
		ComponentId typeId = GetComponentId<TComponent>();
		return RemoveComponent_Internal(entityId, typeId);
	}

	template<typename TComponent>
	inline bool EntityRegistry::HasComponent(EntityId entityId)
	{
		size_t typeId = GetComponentId<TComponent>();
		return HasComponent_Internal(entityId, typeId);
	}

	//template<class ...TComponents>
	//inline bool EntityRegistry::Query(QueryResult& OutResult, const TComponents & ...args)
	//{
	//	std::vector<ComponentId> componentIds;
	//	for (size_t i = 0; i < args.length; i++)
	//	{
	//		componentIds.push_back(GetComponentId(args[i]));
	//	}

	//	return Query_Internal(OutResult, componentIds);
	//}

	// TODO: does this world on all platforms ?
	extern ComponentId s_componentCounter = 0;
	template<typename TComponent>
	inline ComponentId EntityRegistry::GetComponentId() const
	{
		static ComponentId s_componentId = s_componentCounter++;
		return s_componentId;
	}

	/*template<typename T>
	inline ComponentId EntityRegistry::Convert()
	{
		const std::string_view typeName = GetTypeName<T>();
		if (auto typeIdIt = TypeNameToTypeIdMap.find(typeName); typeIdIt != TypeNameToTypeIdMap.end())
		{
			return typeIdIt->second;
			
		}
		
		const ComponentId componentId = TypeNameToTypeIdMap.size();
		TypeNameToTypeIdMap.insert({ typeName,  componentId });
		return componentId;
	}*/
}

#pragma warning(default:4251)