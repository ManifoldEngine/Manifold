#pragma once

#include <ManiMaths/Vec2.h>
#include <ManiMaths/Vec3.h>
#include <ManiMaths/Vec4.h>

namespace Mani
{
	// VEC2
	namespace VEC2I
	{
		constexpr Vec2i 	ZERO	= { 0,  0 };
		constexpr Vec2i 	ONE		= { 1,  1 };
		constexpr Vec2i 	RIGHT	= { 1,  0 };
		constexpr Vec2i 	LEFT	= { -1, 0 };
		constexpr Vec2i 	UP		= { 0,  1 };
		constexpr Vec2i 	DOWN	= { 0, -1 };
	}

	namespace VEC2L
	{
		constexpr Vec2l		ZERO	= { 0,  0 };
		constexpr Vec2l		ONE		= { 1,  1 };
		constexpr Vec2l		RIGHT	= { 1,  0 };
		constexpr Vec2l		LEFT	= { -1, 0 };
		constexpr Vec2l		UP		= { 0,  1 };
		constexpr Vec2l		DOWN	= { 0, -1 };
	}

	namespace VEC2F
	{
		constexpr Vec2f		ZERO	= { 0,  0 };
		constexpr Vec2f		ONE		= { 1,  1 };
		constexpr Vec2f		RIGHT	= { 1,  0 };
		constexpr Vec2f		LEFT	= { -1, 0 };
		constexpr Vec2f		UP		= { 0,  1 };
		constexpr Vec2f		DOWN	= { 0, -1 };
	}

	namespace VEC2D
	{
		constexpr Vec2d		ZERO	= { 0,  0 };
		constexpr Vec2d		ONE		= { 1,  1 };
		constexpr Vec2d		RIGHT	= { 1,  0 };
		constexpr Vec2d		LEFT	= { -1, 0 };
		constexpr Vec2d		UP		= { 0,  1 };
		constexpr Vec2d		DOWN	= { 0, -1 };
	}

	namespace VEC2UI
	{
		constexpr Vec2ui	ZERO	= { 0,  0 };
		constexpr Vec2ui	ONE		= { 1,  1 };
		constexpr Vec2ui	RIGHT	= { 1,  0 };
		constexpr Vec2ui	LEFT	= { -1, 0 };
		constexpr Vec2ui	UP		= { 0,  1 };
		constexpr Vec2ui	DOWN	= { 0, -1 };
	}

	namespace VEC2UL
	{
		constexpr Vec2ul	ZERO	= { 0,  0 };
		constexpr Vec2ul	ONE		= { 1,  1 };
		constexpr Vec2ul	RIGHT	= { 1,  0 };
		constexpr Vec2ul	LEFT	= { -1, 0 };
		constexpr Vec2ul	UP		= { 0,  1 };
		constexpr Vec2ul	DOWN	= { 0, -1 };
	}

	// VEC3
	namespace VEC3I
	{

		constexpr Vec3i 	ZERO	= { 0,  0,  0 };
		constexpr Vec3i 	ONE		= { 1,  1,  1 };
		constexpr Vec3i 	RIGHT	= { 1,  0,  0 };
		constexpr Vec3i 	LEFT	= { -1, 0,  0 };
#if		MANI_COORDINATE_ZPLUSFORWARD_YUP
		constexpr Vec3i 	UP		= { 0,  1,  0 };
		constexpr Vec3i 	DOWN	= { 0, -1,  0 };
		constexpr Vec3i 	FORWARD	= { 0,  0,  1 };
		constexpr Vec3i 	BACK	= { 0,  0, -1 };
#elif	MANI_COORDINATE_ZMINUSFORWARD_YUP
		constexpr Vec3i 	UP		= { 0,  1,  0 };
		constexpr Vec3i 	DOWN	= { 0, -1,  0 };
		constexpr Vec3i 	FORWARD	= { 0,  0, -1 };
		constexpr Vec3i 	BACK	= { 0,  0,  1 };
#elif	MANI_COORDINATE_YPLUSFORWARD_ZUP
		constexpr Vec3i 	UP		= { 0,  0,  1 };
		constexpr Vec3i 	DOWN	= { 0,  0, -1 };
		constexpr Vec3i 	FORWARD	= { 0,  1,  0 };
		constexpr Vec3i 	BACK	= { 0, -1,  0 };
#elif	MANI_COORDINATE_YMINUSFORWARD_ZUP
		constexpr Vec3i 	UP		= { 0,  0,  1 };
		constexpr Vec3i 	DOWN	= { 0,  0, -1 };
		constexpr Vec3i 	FORWARD	= { 0, -1,  0 };
		constexpr Vec3i 	BACK	= { 0,  1,  0 };
#else
		constexpr Vec3i 	UP		= { 0,  1,  0 };
		constexpr Vec3i 	DOWN	= { 0, -1,  0 };
		constexpr Vec3i 	FORWARD = { 0,  0,  1 };
		constexpr Vec3i 	BACK	= { 0,  0, -1 };
#endif
	}

	namespace VEC3L
	{
		constexpr Vec3l		ZERO =	{ 0,  0,  0 };
		constexpr Vec3l		ONE =	{ 1,  1,  1 };
		constexpr Vec3l		RIGHT =	{ 1,  0,  0 };
		constexpr Vec3l		LEFT =	{ -1, 0,  0 };
#if		MANI_COORDINATE_ZPLUSFORWARD_YUP
		constexpr Vec3l 	UP		= { 0,  1,  0 };
		constexpr Vec3l 	DOWN	= { 0, -1,  0 };
		constexpr Vec3l 	FORWARD	= { 0,  0,  1 };
		constexpr Vec3l 	BACK	= { 0,  0, -1 };
#elif	MANI_COORDINATE_ZMINUSFORWARD_YUP
		constexpr Vec3l 	UP		= { 0,  1,  0 };
		constexpr Vec3l 	DOWN	= { 0, -1,  0 };
		constexpr Vec3l 	FORWARD	= { 0,  0, -1 };
		constexpr Vec3l 	BACK	= { 0,  0,  1 };
#elif	MANI_COORDINATE_YPLUSFORWARD_ZUP
		constexpr Vec3l 	UP		= { 0,  0,  1 };
		constexpr Vec3l 	DOWN	= { 0,  0, -1 };
		constexpr Vec3l 	FORWARD	= { 0,  1,  0 };
		constexpr Vec3l 	BACK	= { 0, -1,  0 };
#elif	MANI_COORDINATE_YMINUSFORWARD_ZUP
		constexpr Vec3l 	UP		= { 0,  0,  1 };
		constexpr Vec3l 	DOWN	= { 0,  0, -1 };
		constexpr Vec3l 	FORWARD	= { 0, -1,  0 };
		constexpr Vec3l 	BACK	= { 0,  1,  0 };
#else
		constexpr Vec3l 	UP		= { 0,  1,  0 };
		constexpr Vec3l 	DOWN	= { 0, -1,  0 };
		constexpr Vec3l 	FORWARD = { 0,  0,  1 };
		constexpr Vec3l 	BACK	= { 0,  0, -1 };
#endif
	}

	namespace VEC3F
	{
		constexpr Vec3f		ZERO	= { 0,  0,  0 };
		constexpr Vec3f		ONE		= { 1,  1,  1 };
		constexpr Vec3f		RIGHT	= { 1,  0,  0 };
		constexpr Vec3f		LEFT	= { -1, 0,  0 };
#if		MANI_COORDINATE_ZPLUSFORWARD_YUP
		constexpr Vec3f 	UP		= { 0,  1,  0 };
		constexpr Vec3f 	DOWN	= { 0, -1,  0 };
		constexpr Vec3f 	FORWARD	= { 0,  0,  1 };
		constexpr Vec3f 	BACK	= { 0,  0, -1 };
#elif	MANI_COORDINATE_ZMINUSFORWARD_YUP
		constexpr Vec3f 	UP		= { 0,  1,  0 };
		constexpr Vec3f 	DOWN	= { 0, -1,  0 };
		constexpr Vec3f 	FORWARD	= { 0,  0, -1 };
		constexpr Vec3f 	BACK	= { 0,  0,  1 };
#elif	MANI_COORDINATE_YPLUSFORWARD_ZUP
		constexpr Vec3f 	UP		= { 0,  0,  1 };
		constexpr Vec3f 	DOWN	= { 0,  0, -1 };
		constexpr Vec3f 	FORWARD	= { 0,  1,  0 };
		constexpr Vec3f 	BACK	= { 0, -1,  0 };
#elif	MANI_COORDINATE_YMINUSFORWARD_ZUP
		constexpr Vec3f 	UP		= { 0,  0,  1 };
		constexpr Vec3f 	DOWN	= { 0,  0, -1 };
		constexpr Vec3f 	FORWARD	= { 0, -1,  0 };
		constexpr Vec3f 	BACK	= { 0,  1,  0 };
#else
		constexpr Vec3f 	UP		= { 0,  1,  0 };
		constexpr Vec3f 	DOWN	= { 0, -1,  0 };
		constexpr Vec3f 	FORWARD = { 0,  0,  1 };
		constexpr Vec3f 	BACK	= { 0,  0, -1 };
#endif
	}

	namespace VEC3D
	{
		constexpr Vec3d		ZERO	= { 0,  0,  0 };
		constexpr Vec3d		ONE		= { 1,  1,  1 };
		constexpr Vec3d		RIGHT	= { 1,  0,  0 };
		constexpr Vec3d		LEFT	= { -1,  0,  0 };
#if		MANI_COORDINATE_ZPLUSFORWARD_YUP
		constexpr Vec3d 	UP		= { 0,  1,  0 };
		constexpr Vec3d 	DOWN	= { 0, -1,  0 };
		constexpr Vec3d 	FORWARD	= { 0,  0,  1 };
		constexpr Vec3d 	BACK	= { 0,  0, -1 };
#elif	MANI_COORDINATE_ZMINUSFORWARD_YUP
		constexpr Vec3d 	UP		= { 0,  1,  0 };
		constexpr Vec3d 	DOWN	= { 0, -1,  0 };
		constexpr Vec3d 	FORWARD	= { 0,  0, -1 };
		constexpr Vec3d 	BACK	= { 0,  0,  1 };
#elif	MANI_COORDINATE_YPLUSFORWARD_ZUP
		constexpr Vec3d 	UP		= { 0,  0,  1 };
		constexpr Vec3d 	DOWN	= { 0,  0, -1 };
		constexpr Vec3d 	FORWARD	= { 0,  1,  0 };
		constexpr Vec3d 	BACK	= { 0, -1,  0 };
#elif	MANI_COORDINATE_YMINUSFORWARD_ZUP
		constexpr Vec3d 	UP		= { 0,  0,  1 };
		constexpr Vec3d 	DOWN	= { 0,  0, -1 };
		constexpr Vec3d 	FORWARD	= { 0, -1,  0 };
		constexpr Vec3d 	BACK	= { 0,  1,  0 };
#else
		constexpr Vec3d 	UP		= { 0,  1,  0 };
		constexpr Vec3d 	DOWN	= { 0, -1,  0 };
		constexpr Vec3d 	FORWARD	= { 0,  0,  1 };
		constexpr Vec3d 	BACK	= { 0,  0, -1 };
#endif
	}

	namespace VEC3UI
	{
		constexpr Vec3ui	ZERO = { 0,  0,  0 };
		constexpr Vec3ui	ONE = { 1,  1,  1 };
		constexpr Vec3ui	RIGHT = { 1,  0,  0 };
		constexpr Vec3ui	LEFT = { -1,  0,  0 };
#if		MANI_COORDINATE_ZPLUSFORWARD_YUP
		constexpr Vec3ui 	UP		= { 0,  1,  0 };
		constexpr Vec3ui 	DOWN	= { 0, -1,  0 };
		constexpr Vec3ui 	FORWARD	= { 0,  0,  1 };
		constexpr Vec3ui 	BACK	= { 0,  0, -1 };
#elif	MANI_COORDINATE_ZMINUSFORWARD_YUP
		constexpr Vec3ui 	UP		= { 0,  1,  0 };
		constexpr Vec3ui 	DOWN	= { 0, -1,  0 };
		constexpr Vec3ui 	FORWARD	= { 0,  0, -1 };
		constexpr Vec3ui 	BACK	= { 0,  0,  1 };
#elif	MANI_COORDINATE_YPLUSFORWARD_ZUP
		constexpr Vec3ui 	UP		= { 0,  0,  1 };
		constexpr Vec3ui 	DOWN	= { 0,  0, -1 };
		constexpr Vec3ui 	FORWARD	= { 0,  1,  0 };
		constexpr Vec3ui 	BACK	= { 0, -1,  0 };
#elif	MANI_COORDINATE_YMINUSFORWARD_ZUP
		constexpr Vec3ui 	UP		= { 0,  0,  1 };
		constexpr Vec3ui 	DOWN	= { 0,  0, -1 };
		constexpr Vec3ui 	FORWARD	= { 0, -1,  0 };
		constexpr Vec3ui 	BACK	= { 0,  1,  0 };
#else
		constexpr Vec3ui 	UP		= { 0,  1,  0 };
		constexpr Vec3ui 	DOWN	= { 0, -1,  0 };
		constexpr Vec3ui 	FORWARD	= { 0,  0,  1 };
		constexpr Vec3ui 	BACK	= { 0,  0, -1 };
#endif
	}

	namespace VEC3UL
	{
		constexpr Vec3ul	ZERO	= { 0,  0,  0 };
		constexpr Vec3ul	ONE		= { 1,  1,  1 };
		constexpr Vec3ul	RIGHT	= { 1,  0,  0 };
		constexpr Vec3ul	LEFT	= { -1,  0,  0 };
#if		MANI_COORDINATE_ZPLUSFORWARD_YUP
		constexpr Vec3ul 	UP		= { 0,  1,  0 };
		constexpr Vec3ul 	DOWN	= { 0, -1,  0 };
		constexpr Vec3ul 	FORWARD	= { 0,  0,  1 };
		constexpr Vec3ul 	BACK	= { 0,  0, -1 };
#elif	MANI_COORDINATE_ZMINUSFORWARD_YUP
		constexpr Vec3ul 	UP		= { 0,  1,  0 };
		constexpr Vec3ul 	DOWN	= { 0, -1,  0 };
		constexpr Vec3ul 	FORWARD	= { 0,  0, -1 };
		constexpr Vec3ul 	BACK	= { 0,  0,  1 };
#elif	MANI_COORDINATE_YPLUSFORWARD_ZUP
		constexpr Vec3ul 	UP		= { 0,  0,  1 };
		constexpr Vec3ul 	DOWN	= { 0,  0, -1 };
		constexpr Vec3ul 	FORWARD	= { 0,  1,  0 };
		constexpr Vec3ul 	BACK	= { 0, -1,  0 };
#elif	MANI_COORDINATE_YMINUSFORWARD_ZUP
		constexpr Vec3ul 	UP		= { 0,  0,  1 };
		constexpr Vec3ul 	DOWN	= { 0,  0, -1 };
		constexpr Vec3ul 	FORWARD	= { 0, -1,  0 };
		constexpr Vec3ul 	BACK	= { 0,  1,  0 };
#else
		constexpr Vec3ul 	UP		= { 0,  1,  0 };
		constexpr Vec3ul 	DOWN	= { 0, -1,  0 };
		constexpr Vec3ul 	FORWARD	= { 0,  0,  1 };
		constexpr Vec3ul 	BACK	= { 0,  0, -1 };
#endif
	}

	// VEC4
    namespace VEC4I
    {
        constexpr Vec4i 	ZERO    = { 0,  0,  0,  0 };
        constexpr Vec4i 	ONE     = { 1,  1,  1,  1 };
    }

    namespace VEC4L
    {
        constexpr Vec4l 	ZERO    = { 0,  0,  0,  0 };
        constexpr Vec4l 	ONE     = { 1,  1,  1,  1 };
    }

    namespace VEC4F
    {
        constexpr Vec4f 	ZERO    = { 0,  0,  0,  0 };
        constexpr Vec4f 	ONE     = { 1,  1,  1,  1 };
    }

    namespace VEC4D
    {
        constexpr Vec4d 	ZERO    = { 0,  0,  0,  0 };
        constexpr Vec4d 	ONE     = { 1,  1,  1,  1 };
    }

    namespace VEC4UI
    {
        constexpr Vec4ui 	ZERO    = { 0,  0,  0,  0 };
        constexpr Vec4ui 	ONE     = { 1,  1,  1,  1 };
    }

    namespace VEC4UL
    {
        constexpr Vec4ul 	ZERO    = { 0,  0,  0,  0 };
        constexpr Vec4ul 	ONE     = { 1,  1,  1,  1 };
    }
}