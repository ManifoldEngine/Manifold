#pragma once

#include <Core/Core.h>

#ifdef ECSE_ASSERT_ENABLED
	#define ESCE_ASSERT(CHECK)	if (!(CHECK)) { ECSE_DEBUGBREAK(); }
#else
	#define ESCE_ASSERT(...)
#endif