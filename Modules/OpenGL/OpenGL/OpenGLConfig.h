#pragma once

namespace Mani
{
	constexpr std::string_view OPENGLCONFIG_FILENAME = "OpenGLConfig.json";

	constexpr bool VSYNC_ENABLED = 1;
	constexpr bool VSYNC_DISABLED = 0;

	constexpr bool STBISETFLIPVERTICALLYONLOAD_ENABLED = 1;
	constexpr bool STBISETFLIPVERTICALLYONLOAD_DISABLED = 0;

	struct OpenGLConfig
	{
		bool vsync = VSYNC_ENABLED;
	};
}