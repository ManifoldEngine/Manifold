#pragma once

#include <atomic>

namespace Mani
{
	template<typename T>
	struct Resource
	{
		T value;
		std::atomic<bool> isReady = false;
	};
}