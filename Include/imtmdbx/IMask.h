// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtmdbx
{


/**
 * \brief Interface for bitmask-based data structures backed by MDBX.
 *
 * Represents a sparse bitmask where each "item" is a 64-bit word containing
 * 64 individual bit "units". Items are addressed by offset (word index).
 * The mask supports navigation to find the nearest/next/previous populated offsets.
 */
class IMask: istd::IPolymorphic
{
public:
	/// \brief Gets the bit value at the given absolute position.
	virtual bool GetUnit(quint64 position) = 0;
	/// \brief Sets the bit value at the given absolute position.
	virtual bool SetUnit(quint64 position, bool unit) = 0;

	/// \brief Gets the 64-bit item (word) at the given offset.
	virtual bool GetItem(quint64 offset, quint64& item) = 0;
	/// \brief Sets the 64-bit item (word) at the given offset.
	virtual bool SetItem(quint64 offset, quint64 item) = 0;

	/// \brief Finds the nearest populated offset >= startOffset.
	virtual bool GetNearestOffset(quint64& offset, quint64 startOffset = 0) = 0;
	/// \brief Finds the next populated offset strictly > startOffset.
	virtual bool GetNextItemOffset(quint64& offset, quint64 startOffset = -1) = 0;
	/// \brief Finds the previous populated offset strictly < startOffset.
	virtual bool GetPreviousItemOffset(quint64& offset, quint64 startOffset = 0xffffffffffffff) = 0;
};



}//namespace imtmdbx
