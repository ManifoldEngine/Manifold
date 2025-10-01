#pragma once

#include <Core/ManiAssert.h>
#include <Core/Containers/List.h>

namespace Mani
{
	template<typename K, typename V>
	struct KVP
	{
		K key;
		V value;
	};

	template<typename K, typename V>
	class Map
	{
	public:
		// constructors
		Map() = default;
		Map(const std::initializer_list<std::pair<K, V>>& initializerList)
		{
			for (const auto& [key, value] : initializerList)
			{
				m_keys.add(key);
				m_values.add(value);
			}
		}

		Map(const Map<K, V>& other) : m_keys(other.m_keys), m_values(other.m_values) {}
		Map(Map<K, V>&& other) : m_keys(std::move(other.m_keys)), m_values(std::move(other.m_values)) {}

		Map<K, V>& operator=(const Map<K, V>& other)
		{
			if (this != &other)
			{
				m_keys = other.m_keys;
				m_values = other.m_values;
			}
			return *this;
		}

		Map<K, V>& operator=(Map<K, V>&& other)
		{
			if (this != &other)
			{
				m_keys = std::move(other.m_keys);
				m_values = std::move(other.m_values);
			}
			return *this;
		}

		// capacity
		[[nodiscard]] SizeT count() const { return m_keys.count(); }
		[[nodiscard]] bool isEmpty() const { return m_keys.isEmpty(); }

		// mutators
		void clear() { m_keys.clear(); m_values.clear(); }

		void add(const K& key, const V& value)
		{
			MANI_ASSERT(!m_keys.contains(key), "key already exists");
			m_keys.add(key);
			m_values.add(value);
		}

		void add(const K& key, V&& value)
		{
			MANI_ASSERT(!m_keys.contains(key), "key already exists");
			m_keys.add(key);
			m_values.add(std::forward<V>(value));
		}

		bool remove(const K& key)
		{
			SizeT index = m_keys.indexOf(key);
			if (index == INDEX_NONE)
			{
				return false;
			}
			m_keys.removeAt(index);
			m_values.removeAt(index);
			return true;
		}

		// access
		[[nodiscard]] bool has(const K& key) const
		{
			return m_keys.contains(key);
		}

		[[nodiscard]] V& get(const K& key)
		{
			MANI_ASSERT(has(key), "key does not exists");
			return m_values[m_keys.indexOf(key)];
		}

		[[nodiscard]] const V& get(const K& key) const
		{
			MANI_ASSERT(has(key), "key does not exists");
			return m_values[m_keys.indexOf(key)];
		}

		[[nodiscard]] V& operator[](const K& key)
		{
			if (has(key))
			{
				return get(key);
			}

			add(key, V{});
			return m_values.last();
		}

		[[nodiscard]] const List<K>& keys() const { return m_keys; }
		[[nodiscard]] const List<V>& values() const { return m_values; }

		// iterators
		struct Iterator
		{
			Iterator() = default;
			Iterator(Map* map, SizeT index) : m_map(map), m_index(index) {}

			[[nodiscard]] KVP<K&, V&> operator*();
			[[nodiscard]] bool operator==(const Iterator& other) const;
			[[nodiscard]] bool operator!=(const Iterator& other) const;
			Iterator& operator++();

		private:
			Map* m_map = nullptr;
			SizeT m_index = INDEX_NONE;
		};

		// iterators
		struct IteratorConst
		{
			IteratorConst() = default;
			IteratorConst(const Map* map, SizeT index) : m_map(map), m_index(index) {}

			[[nodiscard]] KVP<const K&, const V&> operator*();
			[[nodiscard]] bool operator==(const IteratorConst& other) const;
			[[nodiscard]] bool operator!=(const IteratorConst& other) const;
			IteratorConst& operator++();

		private:
			const Map* m_map = nullptr;
			SizeT m_index = INDEX_NONE;
		};

		Iterator begin() { return Iterator(this, 0); }
		IteratorConst begin() const { return IteratorConst(this, 0); }
		Iterator end() { return Iterator(this, m_keys.count()); }
		IteratorConst end() const { return IteratorConst(this, m_keys.count()); }

	private:
		List<K> m_keys;
		List<V> m_values;
	};

	template<typename K, typename V>
	KVP<K&, V&> Map<K, V>::Iterator::operator*() 
	{ 
		return {
			.key = m_map->m_keys[m_index],
			.value = m_map->m_values[m_index],
		};
	}

	template<typename K, typename V>
	bool Map<K, V>::Iterator::operator==(const Iterator& other) const
	{
		return m_map == other.m_map && m_index == other.m_index;
	}

	template<typename K, typename V>
	bool Map<K, V>::Iterator::operator!=(const Iterator& other) const
	{
		return m_map != other.m_map || m_index != other.m_index;
	}

	template<typename K, typename V>
	Map<K, V>::Iterator& Map<K, V>::Iterator::operator++()
	{
		m_index++;
		return *this;
	}

	template<typename K, typename V>
	KVP<const K&, const V&> Map<K, V>::IteratorConst::operator*()
	{
		return {
			.key = m_map->m_keys[m_index],
			.value = m_map->m_values[m_index],
		};
	}

	template<typename K, typename V>
	bool Map<K, V>::IteratorConst::operator==(const IteratorConst& other) const
	{
		return m_map == other.m_map && m_index == other.m_index;
	}

	template<typename K, typename V>
	bool Map<K, V>::IteratorConst::operator!=(const IteratorConst& other) const
	{
		return m_map != other.m_map || m_index != other.m_index;
	}

	template<typename K, typename V>
	Map<K, V>::IteratorConst& Map<K, V>::IteratorConst::operator++()
	{
		m_index++;
		return *this;
	}
}