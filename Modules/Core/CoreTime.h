#pragma once

#include <Core/Core.h>

namespace ECSEngine
{
	class Core_API Time
	{
	public:
		friend class Application;

		// returns time since start in milliseconds
		static float getTime();

		// returns time since last frame in milliseconds
		static float getDeltaTime();
	
	private:

		static void onApplicationStart();
		static void onNewFrame();
	};
}
