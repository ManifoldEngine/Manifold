#pragma once

#include "Core.h"
#include "World.h"
#include "Interfaces/ITickable.h"

namespace ECSEngine
{
	class Core_API Application : public ITickable
	{
	public:
		Application();
		~Application();

		void run();
		void stop();

		// Inherited via ITickable
		virtual void tick(float deltaTime) override;
	
		SystemContainer& getSystemContainer();
	private:
		bool m_bIsRunning = true;
		World m_world;

		SystemContainer* m_pSystemContainer = nullptr;
	};
}