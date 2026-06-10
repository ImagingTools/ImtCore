// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtmdbx/IMask.h>
#include <imtmdbx/IDocumentTable.h>


namespace imtmdbx
{


/**
 * \brief Interface for a container that combines multiple IMask instances.
 *
 * Supports AND/OR composition of masks and provides aggregate operations
 * like counting set units or retrieving documents at set positions.
 */
class IMaskContainer: istd::IPolymorphic
{
public:
	/// \brief Adds a mask to the container. Ownership is transferred.
	virtual void AddMask(IMask* mask, bool isInversion = false) = 0;
	/// \brief Clears all masks without deleting them (caller retains ownership after Clear).
	virtual void Clear() = 0;
	/// \brief Removes the last mask from the container.
	virtual bool RemoveLastMask(bool isInversion = false) = 0;
	/// \brief Removes n masks starting at index.
	virtual bool RemoveMask(int index, int n = 1, bool isInversion = false) = 0;
	/// \brief Returns the number of masks in the container.
	virtual int GetMaskCount(bool isInversion = false) = 0;
	/// \brief Sets the document table for document retrieval operations.
	virtual void SetDocumentTable(IDocumentTable* documentTable) = 0;
	/// \brief Returns the total number of set bits across all composed masks.
	virtual quint64 GetUnitCount() = 0;
	/// \brief Returns positions of set bits with pagination support.
	virtual QList<quint64> GetUnitPositions(quint64 offset = 0, quint64 limit = -1) = 0;
	/// \brief Returns documents at positions of set bits with pagination.
	virtual QList<QByteArray> GetDocuments(quint64 offset = 0, quint64 limit = -1) = 0;
};


} // namespace imtmdbx
