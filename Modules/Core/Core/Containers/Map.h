#pragma once

#include <Core/ManiAssert.h>
#include <Core/Containers/List.h>
#include <utility>

namespace Mani
{
	template<typename K, typename V>
	class Map
	{
	public:

		struct Entry
		{
			K key;
			V value;
		};

		using Iterator = List<Entry>::Iterator;
		using IteratorConst = List<Entry>::IteratorConst;

		// constructors
		Map() = default;
		Map(const std::initializer_list<std::pair<K, V>>& initializerList)
		{
			for (const auto& [key, value] : initializerList)
			{
				m_data.add({ key, value });
			}

#if MANI_DEBUG
			List<K> keys;
			for (auto& [key, value] : m_data)
			{
				keys.addUnique(key);
			}
			MANI_ASSERT(keys.count() == m_data.count(), "Initialized a map with duplicated keys");
#endif
		}

		Map(const Map<K, V>& other) : m_data(other.m_data) {}
		Map(Map<K, V>&& other) : m_data(std::move(other.m_data)){}

		Map<K, V>& operator=(const Map<K, V>& other)
		{
			if (this != &other)
			{
				m_data = other.m_data;
			}
			return *this;
		}

		Map<K, V>& operator=(Map<K, V>&& other) noexcept
		{
			if (this != &other)
			{
				m_data = std::move(other.m_data);
			}
			return *this;
		}

		// capacity
		[[nodiscard]] SizeT count() const { return m_data.count(); }
		[[nodiscard]] bool isEmpty() const { return m_data.isEmpty(); }
		void reserve(SizeT inCapacity) { m_data.reserve(inCapacity); }
		void shrink() { m_data.shrink(); }
		void resize(SizeT newSize) { m_data.resize(newSize, Entry()); }

		// mutators
		void clear() { m_data.clear(); }

		void add(const K& key, const V& value)
		{
			MANI_ASSERT(!has(key), "key already exists");
			m_data.add({ key, value });
		}

		void add(const K& key, V&& value)
		{
			MANI_ASSERT(!has(key), "key already exists");
			m_data.add({ key, std::forward<V>(value) });
		}

		bool remove(const K& key)
		{
			SizeT index = indexOfKey(key);
			if (index == INDEX_NONE)
			{
				return false;
			}
			m_data.removeSwapAt(index);
			return true;
		}

		// access
		[[nodiscard]] bool has(const K& key) const
		{
			return indexOfKey(key) != INDEX_NONE;
		}

		[[nodiscard]] V& get(const K& key)
		{
			MANI_ASSERT(has(key), "key does not exists");
			return m_data[indexOfKey(key)].value;
		}

		[[nodiscard]] const V& get(const K& key) const
		{
			MANI_ASSERT(has(key), "key does not exists");
			return m_data[indexOfKey(key)].value;
		}

		[[nodiscard]] V& getOrAdd(const K& key, const V& value)
		{
			const SizeT before = count();
			V& result = operator[](key);
			if (count() > before)
			{
				result = value;
			}
			return result;
		}

		[[nodiscard]] const V* find(const K& key) const
		{
			const SizeT index = indexOfKey(key);
			if (index == INDEX_NONE)
			{
				return nullptr;
			}
			return &m_data[index].value;
		}

		[[nodiscard]] V* find(const K& key)
		{
			const SizeT index = indexOfKey(key);
			if (index == INDEX_NONE)
			{
				return nullptr;
			}
			return &m_data[index].value;
		}

		[[nodiscard]] V& operator[](const K& key)
		{
			const SizeT index = indexOfKey(key);
			if (index != INDEX_NONE)
			{
				return m_data[index].value;
			}

			add(key, V{});
			return m_data.last().value;
		}

		[[nodiscard]] List<K> keys() const 
		{
			List<K> keys;
			for (const auto& [key, value] : m_data)
			{
				keys.add(key);
			}
			return keys;
		}

		[[nodiscard]] List<V> values() const
		{
			List<V> values;
			for (const auto& [key, value] : m_data)
			{
				values.add(value);
			}
			return values;
		}

		Iterator begin() { return m_data.begin(); }
		IteratorConst begin() const { return m_data.begin(); }
		Iterator end() { return m_data.end(); }
		IteratorConst end() const { return m_data.end(); }

	private:
		SizeT indexOfKey(const K& key) const
		{
			return m_data.indexOfIf([&key](const Entry& entry)
			{
				return entry.key == key;
			});
		}

		List<Entry> m_data;
	};
}