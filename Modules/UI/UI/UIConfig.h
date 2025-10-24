#pragma once

#include <cstdint>

namespace Mani
{
	constexpr std::string_view UICONFIG_PATH = "Config/UIConfig.json";

	struct UIConfig
	{
		uint32_t ppu = 1;
		float virtualWidth = 1920.f;
		float virtualHeight = 1080.f;
	};
}