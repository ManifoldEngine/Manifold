#pragma once

#include <Core/Core.h>
#include <ECS/EntityRegistry.h>

namespace Mani
{
	class SystemContainer;

	class World
	{
	public:
		World();
		~World();

		void initialize();
		void deinitialize();
		bool isInitialized() const { return m_isInitialized; }

		virtual void tick(float deltaTime);
		SystemContainer& getSystemContainer();

	private:
		SystemContainer* m_systemContainer = nullptr;
		bool m_isInitialized = false;
	};
}