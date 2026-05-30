// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <array>
#include <cstdint>
#include <limits>

#if __cplusplus >= 202002L // C++20 or newer
#include <bit>
#endif

#if defined(_MSC_VER)
#include <intrin.h>
#endif

// Qt includes
#include <QtCore/QtTypes>

// ACF includes
#include <istd/istd.h>


namespace imtmdbx
{


/// Maximum value for a 64-bit unsigned integer.
inline constexpr quint64 QUINT64_MAX = std::numeric_limits<quint64>::max();


/**
 * \brief Utility functions for bit counting (population count).
 */
namespace BitUtils {


/**
 * \brief Generates a compile-time lookup table for counting bits set in a byte (0-255).
 *
 * Each entry at index i contains the number of bits set to 1 in the binary representation of i.
 * For example, table[5] = 2 since 5 = 0b101 has two bits set.
 */
constexpr std::array<int, 256> MakeBitsSetTable256() {
	std::array<int, 256> table = {};
	for (int i = 0; i < 256; ++i) {
		table[i] = (i & 1) + table[i / 2];
	}

	return table;
}

/// Compile-time initialized lookup table for byte-level popcount.
inline constexpr std::array<int, 256> BitsSetTable256 = MakeBitsSetTable256();

/**
 * \brief Fallback table-based popcount (if no hardware support is available).
 *
 * Counts the number of bits set to 1 in a 64-bit integer using a precomputed lookup table.
 * Processes the integer byte by byte (starting from the lowest), summing the counts.
 *
 * \param n The 64-bit integer to count bits in.
 * \return The number of bits set to 1.
 */
inline int BitCountTable(uint64_t n) {
	return  BitsSetTable256[n & 0xff] +
			BitsSetTable256[(n >> 8) & 0xff] +
			BitsSetTable256[(n >> 16) & 0xff] +
			BitsSetTable256[(n >> 24) & 0xff] +
			BitsSetTable256[(n >> 32) & 0xff] +
			BitsSetTable256[(n >> 40) & 0xff] +
			BitsSetTable256[(n >> 48) & 0xff] +
			BitsSetTable256[(n >> 56)];
}

/**
 * \brief Counts the number of bits set to 1 in a 64-bit integer.
 *
 * Uses hardware acceleration if available:
 *   - std::popcount (C++20)
 *   - __popcnt64 (MSVC)
 *   - __builtin_popcountll (GCC/Clang)
 *
 * Falls back to a lookup table method if no hardware support is available.
 *
 * \param n The 64-bit integer to count bits in.
 * \return The number of bits set to 1.
 */
inline int BitCount(uint64_t n) {
#if __cplusplus >= 202002L
	return std::popcount(n);
#elif defined(_MSC_VER)
	return static_cast<int>(__popcnt64(n));
#elif defined(__GNUG__) || defined(__clang__)
	return __builtin_popcountll(n);
#else
	return BitCountTable(n); // fallback
#endif
}


} // namespace BitUtils


} // namespace imtmdbx
