#pragma once

#include <atomic>

namespace Mani
{
	// unspecialized resource component used to tag resource entities.
	struct ResourceTag 
	{
		uint32_t tag = 0;
	};

	template<typename T>
	struct Resource
	{
		T value;
		std::atomic<bool> isReady = false;
	};
}