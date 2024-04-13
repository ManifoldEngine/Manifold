#pragma once

#include <Core/Core.h>

namespace Mani
{
	class SystemContainer;

	class Application
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
		bool isRunning() const { return m_isRunning; }

	private:
		static Application* s_application;

		bool m_isRunning = false;
		
		SystemContainer* m_systemContainer = nullptr;
	};
}