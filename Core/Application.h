#pragma once

#include "Core.h"
#include "World.h"
#include "Interfaces/ITickable.h"

namespace ECSEngine
{
	class Core_API Application : public ITickable
	{
	public:
		void run();
		void stop();

		// Inherited via ITickable
		virtual void tick(float deltaTime) override;

		void registerTickable(ITickable* pTickable);
		void unRegisterTickable(ITickable* pTickable);
	
	private:
		bool m_bIsRunning = true;
		World m_world;
	};
}