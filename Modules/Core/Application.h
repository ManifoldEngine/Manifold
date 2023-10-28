#pragma once

#include <Core/Core.h>
#include <Core/World.h>
#include <Core/Interfaces/ITickable.h>

namespace ECSEngine
{
	class Core_API Application : public ITickable
	{
	public:
		Application();
		~Application();

		static Application& get();

		void run();
		void stop();

		// Inherited via ITickable
		virtual void tick(float deltaTime) override;
	
		SystemContainer& getSystemContainer();
		bool isRunning() const { return m_bIsRunning; }

	private:
		static Application* sm_pApplication;

		bool m_bIsRunning = true;
		World m_world;

		SystemContainer* m_pSystemContainer = nullptr;
	};
}