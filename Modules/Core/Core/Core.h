#pragma once

#include <Log.h>
#include <string_view>

// Debug utilities
#ifdef MANI_DEBUG
	#ifndef MANI_WEBGL
		#ifdef MANI_WINDOWS
			#include <process.h>
			#define MANI_DEBUGBREAK() std::abort()
		#else	
			#define MANI_DEBUGBREAK()
		#endif
	#endif
	#define MANI_ASSERT_ENABLED 1
#else
	#define MANI_ASSERT_ENABLED 0
#endif

namespace Mani
{
	constexpr LogChannel LogCore("LogCore");
}