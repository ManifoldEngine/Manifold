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

	namespace ECS
	{
		class Registry;
	}

	class SystemContainer;
	
	class SystemBase
	{
	public:
		virtual std::string_view getName() const;
		virtual bool shouldTick(ECS::Registry& registry) const;
		virtual ETickGroup getTickGroup() const;

		void initialize(ECS::Registry& registry, SystemContainer& systemContainer);
		void deinitialize(ECS::Registry& registry);
		
		void enable();
		void disable();

		virtual void tick(float deltaTime, ECS::Registry& registry);
	
		bool isInitialized() const;
		bool isEnabled() const;
	protected:
		virtual void onInitialize(ECS::Registry& registry, SystemContainer& systemContainer);
		virtual void onDeinitialize(ECS::Registry& registry);

		virtual void onEnabled();
		virtual void onDisabled();

	private:
		bool m_isInitialized = false;
		bool m_isEnabled = true;
	};
}
