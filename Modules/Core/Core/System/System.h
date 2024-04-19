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

	class EntityRegistry;
	class SystemContainer;
	
	class SystemBase
	{
	public:
		virtual std::string_view getName() const;
		virtual bool shouldTick(EntityRegistry& registry) const;
		virtual ETickGroup getTickGroup() const;

		void initialize(EntityRegistry& registry, SystemContainer& systemContainer);
		void deinitialize(EntityRegistry& registry);
		
		void enable();
		void disable();

		virtual void tick(float deltaTime, EntityRegistry& registry);
	
		bool isInitialized() const;
		bool isEnabled() const;
	protected:
		virtual void onInitialize(EntityRegistry& registry, SystemContainer& systemContainer);
		virtual void onDeinitialize(EntityRegistry& registry);

		virtual void onEnabled();
		virtual void onDisabled();

	private:
		bool m_isInitialized = false;
		bool m_isEnabled = true;
	};
}
