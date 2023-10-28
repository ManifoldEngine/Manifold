#pragma once

#ifdef Core_EXPORTS
#define Core_API __declspec(dllexport)
#else
#define Core_API __declspec(dllimport)
#endif

// Debug utilities
#ifdef ECSE_DEBUG
	#ifdef ECSE_WINDOWS
		#define ECSE_DEBUGBREAK() __debugbreak()
	#else	
		#define ECSE_DEBUGBREAK()
	#endif
	#define ECSE_ASSERT_ENABLED
#endif