#pragma once

#include <Core/Core.h>

namespace ECSEngine
{
	class SystemContainer;

	class Core_API Application
	{
	public:
		Application();
		~Application();

		static Application& get();

		void run();
		void stop();

		// Inherited via ITickable
		void tick(float deltaTime);
	
		SystemContainer& getSystemContainer();
		bool isRunning() const { return m_bIsRunning; }

	private:
		static Application* sm_pApplication;

		bool m_bIsRunning = false;
		
		SystemContainer* m_pSystemContainer = nullptr;
	};
}