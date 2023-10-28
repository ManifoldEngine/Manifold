#pragma once

#include <Core/Core.h>
#include <functional>

namespace ECSEngine
{
	template<typename ...TArgs>
	class Event
	{
	public:
		struct Handle {
			size_t id = UINT64_MAX;
		};

		Handle subscribe(const std::function<void(TArgs ...)>& f);
		void unsubsribe(const Handle& handle);
		void broadcast(TArgs ... args) const;
		void clear();

		bool isValidHandle(const Handle& handle)
		{
			return handle.id < callbacks.size();
		}

	private:
		std::vector<std::function<void(TArgs ...)>> callbacks;
	};

	template<typename ...TArgs>
	inline Event<TArgs...>::Handle Event<TArgs...>::subscribe(const std::function<void(TArgs ...)>& f)
	{
		callbacks.push_back(f);
		return Handle{ callbacks.size() - 1 };
	}

	template<typename ...TArgs>
	inline void Event<TArgs...>::unsubsribe(const Handle& handle)
	{
		if (isValidHandle(handle))
		{
			callbacks.erase(callbacks.begin() + handle.id);
		}
	}

	template<typename ...TArgs>
	inline void Event<TArgs...>::broadcast(TArgs ...args) const
	{
		for (const auto& f : callbacks)
		{
			f(args ...);
		}
	}

	template<typename ...TArgs>
	inline void Event<TArgs...>::clear()
	{
		callbacks.clear();
	}
}