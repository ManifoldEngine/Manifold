#pragma once

#include <Core/Core.h>
#include <functional>
#include <vector>

namespace Mani
{
	struct EventHandle
	{
#if MANI_WEBGL
		using EventId = unsigned int;
		static const EventId INVALID_EVENT_ID = UINT32_MAX;
#else
		using EventId = size_t;
		static const EventId INVALID_EVENT_ID = UINT64_MAX;
#endif
		EventId id = INVALID_EVENT_ID;
	};

	template<typename ...TArgs>
	class Event
	{
	public:
		Event()
		{
			m_callbacks = new std::vector<std::function<void(TArgs ...)>>();
		}

		~Event()
		{
			delete m_callbacks;
		}

		EventHandle subscribe(const std::function<void(TArgs ...)>& f);
		void unsubscribe(const EventHandle& handle);
		void broadcast(TArgs ... args) const;
		void clear();

		bool isValidHandle(const EventHandle& handle);

	private:
		std::vector<std::function<void(TArgs ...)>>* m_callbacks = nullptr;
	};

	template<typename ...TArgs>
	inline EventHandle Event<TArgs...>::subscribe(const std::function<void(TArgs ...)>& f)
	{
		m_callbacks->push_back(f);
		return EventHandle{ static_cast<EventHandle::EventId>(m_callbacks->size() - 1) };
	};

	template<typename ...TArgs>
	inline void Event<TArgs...>::unsubscribe(const EventHandle& handle)
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
	inline bool Event<TArgs...>::isValidHandle(const EventHandle& handle)
	{
		return handle.id < m_callbacks->size();
	};
}

#if MANI_WINDOWS && !MANI_WEBGL
	#define DECLARE_EVENT(EVENTNAME, ... ) class __declspec(dllexport) EVENTNAME : public Event<__VA_ARGS__> {};
#else
	#define DECLARE_EVENT(EVENTNAME, ... ) class EVENTNAME : public Event<__VA_ARGS__> {};
#endif