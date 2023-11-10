#pragma once

#include <Core/Core.h>
#include <Core/Log.h>

#ifdef ECSE_ASSERT_ENABLED
	#define ECSE_ASSERT(CHECK, MESSAGE)	if (!(CHECK)) { ECSE_LOG_ERROR(Log, MESSAGE); ECSE_DEBUGBREAK(); }
#else
	#define ECSE_ASSERT(...)
#endif