#pragma once

#include <Core/Core.h>
#include <Core/Log.h>
#include <stacktrace>

#ifdef MANI_ASSERT_ENABLED
	#define MANI_ASSERT(CHECK, MESSAGE, ...) \
		if (!(CHECK)) \
		{ \
			const std::string mani_core_formatted_message = std::format(MESSAGE, __VA_ARGS__); \
			MANI_LOG_ERROR("Fatal", "{}\n{}", mani_core_formatted_message, std::to_string(std::stacktrace::current())); \
			MANI_DEBUGBREAK(); \
		}
#else
	#define MANI_ASSERT(...)
#endif