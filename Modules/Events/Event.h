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
			m_pCallbacks = new std::vector<std::function<void(TArgs ...)>>();
		}

		~Event()
		{
			delete m_pCallbacks;
		}

		Handle subscribe(const std::function<void(TArgs ...)>& f);
		void unsubscribe(const Handle& handle);
		void broadcast(TArgs ... args) const;
		void clear();

		bool isValidHandle(const Handle& handle);

	private:
		std::vector<std::function<void(TArgs ...)>>* m_pCallbacks = nullptr;
	};

	template<typename ...TArgs>
	Event<TArgs...>::Handle Event<TArgs...>::subscribe(const std::function<void(TArgs ...)>& f)
	{
		m_pCallbacks->push_back(f);
		return Handle{ m_pCallbacks->size() - 1 };
	};

	template<typename ...TArgs>
	void Event<TArgs...>::unsubscribe(const Handle& handle)
	{
		if (isValidHandle(handle))
		{
			m_pCallbacks->erase(m_pCallbacks->begin() + handle.id);
		}
	};

	template<typename ...TArgs>
	void Event<TArgs...>::broadcast(TArgs ... args) const
	{
		for (const auto& f : *m_pCallbacks)
		{
			f(args ...);
		}
	};

	template<typename ...TArgs>
	void Event<TArgs...>::clear()
	{
		m_pCallbacks->clear();
	};

	template<typename ...TArgs>
	bool Event<TArgs...>::isValidHandle(const Handle& handle)
	{
		return handle.id < m_pCallbacks->size();
	};
}

#ifdef ECSE_WINDOWS
	#define DECLARE_EVENT(EVENTNAME, ... ) class __declspec(dllexport) EVENTNAME : public Event<__VA_ARGS__> {};
#else
	#define DECLARE_EVENT(EVENTNAME, ... ) class EVENTNAME : public Event<__VA_ARGS__> {};
#endif