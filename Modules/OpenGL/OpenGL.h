#pragma once

#ifdef OpenGL_EXPORTS
#define OpenGL_API __declspec(dllexport)
#else
#define OpenGL_API __declspec(dllimport)
#endif
