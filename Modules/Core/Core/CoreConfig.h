#pragma once

namespace Mani
{
	struct CoreConfig
	{
		size_t threadPoolSize = 0;

		// windowed application settings
		std::string startupWindowTitle = "Manifold";
		uint32_t startupScreenWidth = 1280;
		uint32_t startupScreenHeight = 720;
	};
}