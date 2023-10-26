#pragma once

#include <Core/Core.h>
#include <string_view>

namespace ECSEngine
{
	class EntityRegistry;
	class SystemContainer;
	
	class Core_API SystemBase
	{
	public:
		virtual std::string_view getName() const;
		virtual float getFrameTime() const;
		virtual bool shouldTick(EntityRegistry& entityRegistry) const;

		void initialize(EntityRegistry& entityRegistry, SystemContainer& systemContainer);
		void deinitialize(EntityRegistry& entityRegistry);

		virtual void tick(float deltaTime, EntityRegistry& entityRegistry);
	
	protected:
		virtual void onInitialize(EntityRegistry& entityRegistry, SystemContainer& systemContainer);
		virtual void onDeinitialize(EntityRegistry& entityRegistry);

	private:
		bool m_bIsInitialized = false;
	};
}
