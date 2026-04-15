#pragma once

#include <Core/Core.h>
#include <Core/Log.h>
#include <stacktrace>

namespace Mani
{
	constexpr LogChannel LogFatal("Fatal");
}

#if MANI_ASSERT_ENABLED
	#define MANI_ASSERT(CHECK, ...) \
		do \
		{ \
			if (!(CHECK)) \
			{ \
				MANI_LOG_ERROR(Mani::LogFatal, __VA_ARGS__); \
				MANI_LOG_ERROR(Mani::LogFatal, "{}", std::to_string(std::stacktrace::current())); \
				MANI_DEBUGBREAK(); \
			} \
		} while (false); \

#else
	#define MANI_ASSERT(...)
#endif