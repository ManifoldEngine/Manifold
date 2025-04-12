#pragma once

#include <Core/Core.h>
#include <string_view>

namespace Mani
{
	enum class ETickGroup : uint8_t
	{
		PreTick = 0,
		Tick = 1,
		PostTick = 2,
	};

	class World;

	namespace ECS
	{
		class Registry;
	}
		
	class SystemBase
	{
	public:
		virtual std::string_view getName() const;
		virtual bool shouldTick(ECS::Registry& registry) const;
		virtual ETickGroup getTickGroup() const;

		void initialize(ECS::Registry& registry, World& world);
		void deinitialize(ECS::Registry& registry);

		virtual void tick(float deltaTime, ECS::Registry& registry);
	
		bool isInitialized() const;
	protected:
		virtual void onInitialize(ECS::Registry& registry, World& world);
		virtual void onDeinitialize(ECS::Registry& registry);

	private:
		bool m_isInitialized = false;
	};
}
