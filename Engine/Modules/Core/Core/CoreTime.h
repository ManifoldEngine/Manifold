#pragma once

#include <Core/Core.h>
#include <string_view>

namespace Mani
{
	class Time
	{
	public:
		friend class Application;

		// returns time since start in milliseconds
		static float getTimeSinceStart();

		// returns time since last frame in milliseconds
		static float getDeltaTime();

		static std::string getTimeFormatted();
	
	private:

		static void onNewFrame();
		static void onApplicationStart();
	};
}
