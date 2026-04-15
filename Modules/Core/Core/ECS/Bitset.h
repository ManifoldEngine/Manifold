#pragma once

#include <Core/ManiTypes.h>
#include <array>

namespace Mani
{
	using BitsetIndexType = unsigned int;

	template<BitsetIndexType TBits>
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
			for (BitsetIndexType i = 0; i < wordCount(); ++i)
			{
				if (m_bits[i] > 0)
				{
					return true;
				}
			}
			return false;
		};

		bool test(const BitsetIndexType index) const
		{
			if (index >= TBits)
			{
				return false;
			}
			return (m_bits[index / wordSize()] & (1 << index % wordSize())) > 0;
		};

		Bitset& set(const BitsetIndexType index, bool value = true)
		{
			if (index >= TBits)
			{
				return *this;
			}

			const BitsetIndexType wordIndex = index / wordSize();
			BitsetIndexType& word = m_bits[wordIndex];
			const BitsetIndexType bit = 1 << index % wordSize();

			if (value)
			{
				word |= bit;
				m_count++;
			}
			else
			{
				word &= ~bit;
				m_count--;
			}

			return *this;
		};

		Bitset& reset()
		{
			for (BitsetIndexType i = 0; i < wordCount(); ++i)
			{
				m_bits[i] = 0;
			}
			m_count = 0;
			return *this;
		};

		bool contains(const Bitset& rhs) const
		{
			return rhs.operator==(rhs & *this);
		}

		SizeT count() const { return m_count; }

		bool operator==(const Bitset& rhs) const
		{
			for (BitsetIndexType i = 0; i < wordCount(); ++i)
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
			for (BitsetIndexType i = 0; i < wordCount(); ++i)
			{
				m_bits[i] &= rhs.m_bits[i];
			}
			return *this;
		};

		Bitset& operator|=(const Bitset& rhs)
		{
			for (BitsetIndexType i = 0; i < wordCount(); ++i)
			{
				m_bits[i] |= rhs.m_bits[i];
			}
			return *this;
		};

		Bitset& operator^=(const Bitset& rhs)
		{
			for (BitsetIndexType i = 0; i < wordCount(); ++i)
			{
				m_bits[i] ^= rhs.m_bits[i];
			}
			return *this;
		};

	private:
		constexpr BitsetIndexType wordSize() const { return sizeof(BitsetIndexType) * 8; };
		constexpr BitsetIndexType wordCount() const { return (TBits / wordSize()) + 1; };
		std::array<BitsetIndexType, (TBits / (sizeof(BitsetIndexType) * 8)) + 1> m_bits;
		SizeT m_count = 0;
	};

	template<BitsetIndexType TBits>
	requires (TBits > 0)
	Bitset<TBits> operator|(const Bitset<TBits>& lhs, const Bitset<TBits>& rhs)
	{
		Bitset<TBits> result = lhs;
		result |= rhs;
		return result;
	};

	template<BitsetIndexType TBits>
	requires (TBits > 0)
	Bitset<TBits> operator&(const Bitset<TBits>& lhs, const Bitset<TBits>& rhs)
	{
		Bitset<TBits> result = lhs;
		result &= rhs;
		return result;
	};

	template<BitsetIndexType TBits>
	requires (TBits > 0)
	Bitset<TBits> operator^(const Bitset<TBits>& lhs, const Bitset<TBits>& rhs)
	{
		Bitset<TBits> result = lhs;
		result ^= rhs;
		return result;
	};
}