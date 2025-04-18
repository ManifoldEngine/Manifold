#pragma once

#include <array>

namespace Mani
{
	template<unsigned int TBits>
	requires (TBits > 0)
	class Bitset
	{
	public:
		Bitset()
		{
			reset();
		};

		bool any() const
		{
			for (unsigned int i = 0; i < wordCount(); ++i)
			{
				if (m_bits[i] > 0)
				{
					return true;
				}
			}
			return false;
		};

		bool test(const unsigned int index) const
		{
			if (index >= TBits)
			{
				return false;
			}
			return (m_bits[index / wordSize()] & (1 << index % wordSize())) > 0;
		};

		Bitset& set(const unsigned int index, bool value = true)
		{
			if (index >= TBits)
			{
				return *this;
			}

			const unsigned int wordIndex = index / wordSize();
			unsigned int& word = m_bits[wordIndex];
			const unsigned int bit = 1 << index % wordSize();

			if (value)
			{
				word |= bit;
			}
			else
			{
				word &= ~bit;
			}

			return *this;
		};

		Bitset& reset()
		{
			for (unsigned int i = 0; i < wordCount(); ++i)
			{
				m_bits[i] = 0;
			}
			return *this;
		};

		bool operator==(const Bitset& rhs) const
		{
			for (unsigned int i = 0; i < wordCount(); ++i)
			{
				if (m_bits[i] != rhs.m_bits[i])
				{
					return false;
				}
			}
			return true;
		};

		Bitset& operator&=(const Bitset& rhs)
		{
			for (unsigned int i = 0; i < wordCount(); ++i)
			{
				m_bits[i] &= rhs.m_bits[i];
			}
			return *this;
		};

		Bitset& operator|=(const Bitset& rhs)
		{
			for (unsigned int i = 0; i < wordCount(); ++i)
			{
				m_bits[i] |= rhs.m_bits[i];
			}
			return *this;
		};

		Bitset& operator^=(const Bitset& rhs)
		{
			for (unsigned int i = 0; i < wordCount(); ++i)
			{
				m_bits[i] ^= rhs.m_bits[i];
			}
			return *this;
		};

	private:
		constexpr unsigned int wordSize() const { return sizeof(unsigned int) * 8; };
		constexpr unsigned int wordCount() const { return (TBits / wordSize()) + 1; };
		std::array<unsigned int, (TBits / (sizeof(unsigned int) * 8)) + 1> m_bits;
	};

	template<unsigned int TBits>
	requires (TBits > 0)
	Bitset<TBits> operator|(const Bitset<TBits>& lhs, const Bitset<TBits>& rhs)
	{
		Bitset<TBits> result = lhs;
		result |= rhs;
		return result;
	};

	template<unsigned int TBits>
	requires (TBits > 0)
	Bitset<TBits> operator&(const Bitset<TBits>& lhs, const Bitset<TBits>& rhs)
	{
		Bitset<TBits> result = lhs;
		result &= rhs;
		return result;
	};

	template<unsigned int TBits>
	requires (TBits > 0)
	Bitset<TBits> operator^(const Bitset<TBits>& lhs, const Bitset<TBits>& rhs)
	{
		Bitset<TBits> result = lhs;
		result ^= rhs;
		return result;
	};
}