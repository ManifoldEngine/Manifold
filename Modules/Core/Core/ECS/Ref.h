#pragma once

#include <Core/ECS/Entity.h>
#include <Core/ECS/Archetype.h>
#include <Core/ManiTypes.h>

#if MANI_ASSERT_ENABLED
#include <ManiZ/Reflection.h>
#endif

namespace Mani
{
	namespace ECS
	{
		template<typename T, typename TRegistry>
		struct ComponentRef
		{
			static constexpr ComponentRef INVALID() { return {}; }

			ComponentRef() = default;

			ComponentRef(T* v, const TRegistry& registry, const Archetype& archetype) :
				m_ptr(v),
				m_registry(&registry),
				m_archetype(&archetype),

				m_version(registry.getVersion()),
				m_archetypeVersion(archetype.getVersion())
			{
			}

			T& operator*() const
			{
				MANI_ASSERT(isValid(), DANGLING_REF_MESSAGE, TYPE_NAME, TYPE_NAME);
				return *m_ptr;
			}

			T* operator->() const
			{
				MANI_ASSERT(isValid(), DANGLING_REF_MESSAGE, TYPE_NAME, TYPE_NAME);
				return m_ptr;
			}

			operator bool() const
			{
				return isValid();
			}

			T* get() const
			{
				MANI_ASSERT(isValid(), DANGLING_REF_MESSAGE, TYPE_NAME, TYPE_NAME);
				return m_ptr;
			}

			bool isValid() const
			{
				return m_ptr != nullptr
					&& m_version == m_registry->getVersion()
					&& m_archetypeVersion == m_archetype->getVersion();
			}

		private:
			T* m_ptr = nullptr;
			const TRegistry* m_registry = nullptr;
			const Archetype* m_archetype = nullptr;

			SizeT m_version = 0;
			SizeT m_archetypeVersion = 0;

			inline static constexpr std::string_view DANGLING_REF_MESSAGE = "Dangling reference, get a new Ref to {} from the registry, or make a LazyRef<{}> (warning, slow)";
#if MANI_ASSERT_ENABLED
			inline static constexpr std::string_view TYPE_NAME = ManiZ::RFL::getTypeName<T>();
#else
			inline static constexpr std::string_view TYPE_NAME = "[UNDEFINED]";
#endif
		};
	
		template<typename T, typename TRegistry>
		struct LazyComponentRef
		{
			LazyComponentRef(ECS::EntityId entityId, TRegistry& registry) :
				m_entityId(entityId),
				m_registry(&registry)
			{
				MANI_ASSERT(isValid(), "Lazy referece is invalid at construction time");
			}

			T& operator*() const
			{
				return m_registry->get<T>(m_entityId)->value;
			}

			ComponentRef<T, TRegistry> operator->() const
			{
				return m_registry->get<T>(m_entityId);
			}

			operator bool() const
			{
				return isValid();
			}

			bool isValid() const
			{
				return m_registry->isValid(m_entityId) && m_registry->template has<T>(m_entityId);
			}

		private:
			ECS::EntityId m_entityId = ECS::INVALID_ID;
			TRegistry* m_registry = nullptr;
		};
	}
}