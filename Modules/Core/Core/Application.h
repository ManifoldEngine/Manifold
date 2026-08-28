#pragma once

#include <Core/Core.h>
#include <Core/World.h>
#include <Core/CoreConfig.h>
#include <Core/Async/ThreadPool.h>
#include <Core/Async/Defer.h>
#include <Core/Profiling/Profiler.h>

#define MANI_ASSERT_APP_THREAD MANI_ASSERT(Mani::Application::get().getThreadId() == Mani::thisThreadId(), "Only allowed on the main thread");

namespace Mani
{
	class Application
	{
	public:
		static constexpr size_t THREAD_COUNT = 10;

		Application();
		~Application();

		static Application& get();
		static bool exists();

		void run();
		void stop();

		void tick();
	
		World& getWorld() { return m_world; }
		ThreadPool& getThreadPool() { return m_threadPool; }
		Profiler& getProfiler() { return m_profiler; }
		Deferred& getDeferred() { return m_deferred; }
		const CoreConfig& getConfig() const { return m_config; }
		ThreadId getThreadId() const { return m_threadId; }

		bool isRunning() const { return m_isRunning; }

	private:
		static Application* s_application;

		bool m_isRunning = false;
		
		World m_world;
		ThreadPool m_threadPool;
		Profiler m_profiler;
		Deferred m_deferred;
		CoreConfig m_config;

		ThreadId m_threadId;
	};
}