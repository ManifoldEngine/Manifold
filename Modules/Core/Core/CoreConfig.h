#pragma once

namespace Mani
{
	constexpr std::string_view CONFIG_FILENAME = "CoreConfig.json";

	struct CoreConfig
	{
		size_t threadPoolSize = 0;
		uint32_t targetFPS = 144;
	};
}