#pragma once

#include <process.h>
#include <string_view>

#ifdef Core_EXPORTS
#define Core_API __declspec(dllexport)
#else
#define Core_API __declspec(dllimport)
#endif

// Debug utilities
#ifdef ECSE_DEBUG
	#ifdef ECSE_WINDOWS
		#define ECSE_DEBUGBREAK() std::abort()
	#else	
		#define ECSE_DEBUGBREAK()
	#endif
	#define ECSE_ASSERT_ENABLED
#endif

namespace ECSEngine
{
	const std::string_view LogCore = "LogCore";
}