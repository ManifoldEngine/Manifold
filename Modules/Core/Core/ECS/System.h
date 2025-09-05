#pragma once

#include <Core/Core.h>
#include <string_view>

namespace Mani
{
	enum class ETickGroup : uint8_t
	{
		Begin		= 0,

		PreInput	= 1,
		Input		= 2,
		PostInput	= 3,

		PreUpdate	= 4,
		Update		= 5,
		PostUpdate	= 6,

		PreRender	= 7,
		Render		= 8,
		PostRender	= 9,

		End			= 10,
	};

	class World;

	namespace ECS
	{
		class Registry;

		class System
		{
		public:
			virtual std::string_view getName() const;
			virtual bool shouldTick(ECS::Registry& registry) const;
			virtual ETickGroup getTickGroup() const;

			void initialize(ECS::Registry& registry, World& world);
			void deinitialize(ECS::Registry& registry, World& world);

			virtual void tick(ECS::Registry& registry);

			bool isInitialized() const;
		protected:
			virtual void onInitialize(ECS::Registry& registry, World& world);
			virtual void onDeinitialize(ECS::Registry& registry, World& world);

		private:
			bool m_isInitialized = false;
		};
	}
}
