#pragma once

#include <Core/Core.h>
#include <ECS/EntityRegistry.h>

namespace ECSEngine
{
	class SystemContainer;

	class Core_API World
	{
	public:
		World();
		~World();

		void initialize();
		void deinitialize();
		bool isInitialized() const { return m_bIsInitialized; }

		virtual void tick(float deltaTime);
		SystemContainer& getSystemContainer();

	private:
		SystemContainer* m_pSystemContainer = nullptr;
		bool m_bIsInitialized = false;
	};
}