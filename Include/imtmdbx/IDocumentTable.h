// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtmdbx
{


class IDocumentTable: istd::IPolymorphic
{
public:
	virtual quint64 AddDocument(const QByteArray& data) = 0;
	virtual quint64 AddDocument(quint64 data) = 0;
	virtual quint64 AddDocument(const QByteArray& key, const QByteArray& data) = 0;
	virtual QByteArray GetDocument() = 0;
	virtual QByteArray GetDocument(quint64 key) = 0;
	virtual QByteArray GetDocument(const QByteArray& key) = 0;
	virtual bool GetKey(quint64& key) const = 0;
	virtual bool GetKey(QByteArray& key) const = 0;
	virtual bool HasRecord(quint64 key) = 0;
	virtual bool HasRecord(const QByteArray& key) = 0;
	virtual bool UpdateDocument(quint64 key, const QByteArray& data) = 0;
	virtual bool UpdateDocument(const QByteArray& key, const QByteArray& data) = 0;
	virtual bool GetKey(quint64& key, const QByteArray& value = QByteArray()) = 0;
	virtual bool GetFirstKey(quint64& key) = 0;
	virtual bool GetLastKey(quint64& key) = 0;
	virtual QByteArray GetKeyBA(const QByteArray& value = QByteArray()) = 0;
	virtual bool CreateIndex() = 0;
	virtual bool MoveTo(quint64 key) = 0;
	virtual bool MoveTo(const QByteArray& key) = 0;
	virtual bool MoveToFirst() = 0;
	virtual bool MoveToLast() = 0;
	virtual bool MoveToLowerBound(quint64 key) = 0;
	virtual bool MoveToLowerBound(const QByteArray& key) = 0;
	virtual bool MoveToUpperBound(quint64 key) = 0;
	virtual bool MoveToUpperBound(const QByteArray& key) = 0;
	virtual bool MoveToNext() = 0;
	virtual bool MoveToPrevious() = 0;
	virtual bool MoveToValue(const QByteArray& value) = 0;
	virtual bool RemoveDocument() = 0;
	virtual bool RemoveDocument(quint64 key, const QByteArray& value = QByteArray()) = 0;
	virtual bool RemoveDocument(const QByteArray& key, const QByteArray& value = QByteArray()) = 0;
};


}//namespace imtmdbx

