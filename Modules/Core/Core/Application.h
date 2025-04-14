#pragma once

#include <Core/Core.h>
#include <Core/World/World.h>
#include <Core/Thread/ThreadPool.h>

namespace Mani
{
	class Application
	{
	public:
		static constexpr size_t THREAD_COUNT = 10;

		Application();
		~Application();

		static Application& get();

		void run();
		void stop();

		void tick(float deltaTime);
	
		World& getWorld() { return m_world; }
		ThreadPool& getThreadPool() { return m_threadPool; }

		bool isRunning() const { return m_isRunning; }

	private:
		static Application* s_application;

		bool m_isRunning = false;
		
		World m_world;
		ThreadPool m_threadPool;
	};
}