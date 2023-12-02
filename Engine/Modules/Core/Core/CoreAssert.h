#pragma once

#include <Core/Core.h>
#include <Core/Log.h>

#ifdef MANI_ASSERT_ENABLED
	#define MANI_ASSERT(CHECK, MESSAGE)	if (!(CHECK)) { MANI_LOG_ERROR(Log, MESSAGE); MANI_DEBUGBREAK(); }
#else
	#define MANI_ASSERT(...)
#endif