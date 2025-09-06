#pragma once

namespace Mani
{
	constexpr std::string_view OPENGLCONFIG_FILENAME = "OpenGLConfig.json";

	constexpr uint8_t VSYNC_ENABLED = 1;
	constexpr uint8_t VSYNC_DISABLED = 0;

	constexpr uint8_t STBISETFLIPVERTICALLYONLOAD_ENABLED = 1;
	constexpr uint8_t STBISETFLIPVERTICALLYONLOAD_DISABLED = 0;

	struct OpenGLConfig
	{
		uint8_t vsync = VSYNC_ENABLED;
		uint8_t stbiSetFlipVerticallyOnLoad = STBISETFLIPVERTICALLYONLOAD_DISABLED;
	};
}