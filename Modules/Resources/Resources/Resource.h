#pragma once

#include <memory>

namespace Mani
{
	template<typename T>
	struct Resource
	{
		std::unique_ptr<T> value = nullptr;
		std::atomic<bool> isReady = false;
		const T& get() const { return *value.get(); }
		T& getMutable() { return *value.get(); }
	};
}