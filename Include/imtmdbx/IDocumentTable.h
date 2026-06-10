// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtmdbx
{


/**
 * \brief Interface for MDBX-backed document table operations.
 *
 * Provides a cursor-based API for storing, retrieving, updating, and removing
 * documents (key-value pairs) within a named MDBX map. Supports both integer
 * and byte-array keys, optional secondary index, and cursor navigation.
 */
class IDocumentTable: istd::IPolymorphic
{
public:
	/// \brief Appends a document with an auto-incremented integer key.
	virtual quint64 AddDocument(const QByteArray& data) = 0;
	/// \brief Appends a 64-bit integer value with an auto-incremented key.
	virtual quint64 AddDocument(quint64 data) = 0;
	/// \brief Adds a document with a specified byte-array key.
	virtual quint64 AddDocument(const QByteArray& key, const QByteArray& data) = 0;

	/// \brief Gets the document value at the current cursor position.
	virtual QByteArray GetDocument() = 0;
	/// \brief Gets the document value for the given integer key.
	virtual QByteArray GetDocument(quint64 key) = 0;
	/// \brief Gets the document value for the given byte-array key.
	virtual QByteArray GetDocument(const QByteArray& key) = 0;

	/// \brief Gets the integer key at the current cursor position.
	virtual bool GetKey(quint64& key) const = 0;
	/// \brief Gets the byte-array key at the current cursor position.
	virtual bool GetKey(QByteArray& key) const = 0;

	/// \brief Checks whether a record with the given integer key exists.
	virtual bool HasRecord(quint64 key) = 0;
	/// \brief Checks whether a record with the given byte-array key exists.
	virtual bool HasRecord(const QByteArray& key) = 0;

	/// \brief Updates the document at the given integer key.
	virtual bool UpdateDocument(quint64 key, const QByteArray& data) = 0;
	/// \brief Updates the document at the given byte-array key.
	virtual bool UpdateDocument(const QByteArray& key, const QByteArray& data) = 0;

	/// \brief Finds the integer key for a given value (linear scan if no index).
	virtual bool GetKey(quint64& key, const QByteArray& value = QByteArray()) = 0;
	/// \brief Gets the first (smallest) integer key in the table.
	virtual bool GetFirstKey(quint64& key) = 0;
	/// \brief Gets the last (largest) integer key in the table.
	virtual bool GetLastKey(quint64& key) = 0;
	/// \brief Finds the byte-array key for a given value.
	virtual QByteArray GetKeyBA(const QByteArray& value = QByteArray()) = 0;

	/// \brief Creates a secondary index for value-based lookups.
	virtual bool CreateIndex() = 0;

	/// \brief Moves the cursor to the record with the given integer key.
	virtual bool MoveTo(quint64 key) = 0;
	/// \brief Moves the cursor to the record with the given byte-array key.
	virtual bool MoveTo(const QByteArray& key) = 0;
	/// \brief Moves the cursor to the first record.
	virtual bool MoveToFirst() = 0;
	/// \brief Moves the cursor to the last record.
	virtual bool MoveToLast() = 0;
	/// \brief Moves the cursor to the lower bound of the given integer key.
	virtual bool MoveToLowerBound(quint64 key) = 0;
	/// \brief Moves the cursor to the lower bound of the given byte-array key.
	virtual bool MoveToLowerBound(const QByteArray& key) = 0;
	/// \brief Moves the cursor to the upper bound of the given integer key.
	virtual bool MoveToUpperBound(quint64 key) = 0;
	/// \brief Moves the cursor to the upper bound of the given byte-array key.
	virtual bool MoveToUpperBound(const QByteArray& key) = 0;
	/// \brief Moves the cursor to the next record.
	virtual bool MoveToNext() = 0;
	/// \brief Moves the cursor to the previous record.
	virtual bool MoveToPrevious() = 0;
	/// \brief Moves the cursor to the record with the given value.
	virtual bool MoveToValue(const QByteArray& value) = 0;

	/// \brief Removes the document at the current cursor position.
	virtual bool RemoveDocument() = 0;
	/// \brief Removes the document with the given integer key (and optionally a specific value in multi-mode).
	virtual bool RemoveDocument(quint64 key, const QByteArray& value = QByteArray()) = 0;
	/// \brief Removes the document with the given byte-array key.
	virtual bool RemoveDocument(const QByteArray& key, const QByteArray& value = QByteArray()) = 0;

	/// \brief Reopens cursors (e.g. after a transaction reset/renew).
	virtual void Reopen() = 0;
};


}//namespace imtmdbx

