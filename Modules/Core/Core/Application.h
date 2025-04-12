#pragma once

#include <Core/Core.h>
#include <Core/System/SystemContainer.h>
#include <Core/Thread/ThreadPool.h>

namespace Mani
{
	class Application
	{
	public:
		Application();
		~Application();

		static Application& get();

		void run();
		void stop();

		void tick(float deltaTime);
	
		SystemContainer& getSystemContainer() { return m_systemContainer; }
		ThreadPool& getThreadPool() { return m_threadPool; }

		bool isRunning() const { return m_isRunning; }

	private:
		static Application* s_application;

		bool m_isRunning = false;
		
		SystemContainer m_systemContainer;
		ThreadPool m_threadPool;
	};
}