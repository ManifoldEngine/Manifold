#pragma once

#include <string_view>

// Debug utilities
#ifdef MANI_DEBUG
	#ifndef __EMSCRIPTEN__
		#ifdef MANI_WINDOWS
			#include <process.h>
			#define MANI_DEBUGBREAK() std::abort()
		#else	
			#define MANI_DEBUGBREAK()
		#endif
		#define MANI_ASSERT_ENABLED
	#endif
#endif

namespace Mani
{
	const std::string_view LogCore = "LogCore";
}