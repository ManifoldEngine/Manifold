#pragma once

#include <string_view>

#ifdef Camera_EXPORTS
#define Camera_API __declspec(dllexport)
#else
#define Camera_API __declspec(dllimport)
#endif

namespace ECSEngine
{
	const std::string_view LogCamera = "LogCamera";
}