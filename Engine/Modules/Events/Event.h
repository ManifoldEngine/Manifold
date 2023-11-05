#pragma once

#include <Core/Core.h>
#include <functional>
#include <vector>

namespace ECSEngine
{
	template<typename ...TArgs>
	class Event
	{
	public:
		struct Handle {
			size_t id = UINT64_MAX;
		};

		Event()
		{
			m_callbacks = new std::vector<std::function<void(TArgs ...)>>();
		}

		~Event()
		{
			delete m_callbacks;
		}

		Handle subscribe(const std::function<void(TArgs ...)>& f);
		void unsubscribe(const Handle& handle);
		void broadcast(TArgs ... args) const;
		void clear();

		bool isValidHandle(const Handle& handle);

	private:
		std::vector<std::function<void(TArgs ...)>>* m_callbacks = nullptr;
	};

	template<typename ...TArgs>
	inline Event<TArgs...>::Handle Event<TArgs...>::subscribe(const std::function<void(TArgs ...)>& f)
	{
		m_callbacks->push_back(f);
		return Handle{ m_callbacks->size() - 1 };
	};

	template<typename ...TArgs>
	inline void Event<TArgs...>::unsubscribe(const Handle& handle)
	{
		if (isValidHandle(handle))
		{
			m_callbacks->erase(m_callbacks->begin() + handle.id);
		}
	};

	template<typename ...TArgs>
	inline void Event<TArgs...>::broadcast(TArgs ... args) const
	{
		for (const auto& f : *m_callbacks)
		{
			if (f)
			{
				f(args ...);
			}
		}
	};

	template<typename ...TArgs>
	inline void Event<TArgs...>::clear()
	{
		m_callbacks->clear();
	};

	template<typename ...TArgs>
	inline bool Event<TArgs...>::isValidHandle(const Handle& handle)
	{
		return handle.id < m_callbacks->size();
	};
}

#ifdef ECSE_WINDOWS
	#define DECLARE_EVENT(EVENTNAME, ... ) class __declspec(dllexport) EVENTNAME : public Event<__VA_ARGS__> {};
#else
	#define DECLARE_EVENT(EVENTNAME, ... ) class EVENTNAME : public Event<__VA_ARGS__> {};
#endif