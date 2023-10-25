#pragma once

#include "Core.h"
#include "Interfaces/IWorldObject.h"
#include "Interfaces/ITickable.h"
#include <string_view>
#include <memory>


namespace ECSEngine
{
	class EntityRegistry;
	
	class Core_API SystemBase : public IWorldObject
	{
	public:
		virtual std::string_view getName() const;
		virtual float getFrameTime() const;
		virtual bool shouldTick(EntityRegistry& entityRegistry) const;

		void initialize(World* inpWorld, EntityRegistry& entityRegistry);
		void deinitialize(EntityRegistry& entityRegistry);

		// Inherited via IWorldObject
		World* getWorld() const override;

		virtual void tick(float deltaTime, EntityRegistry& entityRegistry);
	
	protected:
		virtual void onInitialize(EntityRegistry& entityRegistry);
		virtual void onDeinitialize(EntityRegistry& entityRegistry);
	
	private:
		World* pWorld;
	};
}
