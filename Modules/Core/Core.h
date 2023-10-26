#pragma once

#ifdef Core_EXPORTS
#define Core_API __declspec(dllexport)
#else
#define Core_API __declspec(dllimport)
#endif
