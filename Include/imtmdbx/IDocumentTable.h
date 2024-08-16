#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QByteArray>

#include <istd/IPolymorphic.h>


namespace imtmdbx
{

class IDocumentTable: istd::IPolymorphic
{
public:
	virtual qint64 AddDocument(const QByteArray& data) = 0;
	virtual qint64 AddDocument(qint64 data) = 0;
	virtual qint64 AddDocument(const QByteArray& key, const QByteArray& data) = 0;
	virtual QByteArray GetDocument() = 0;
	virtual QByteArray GetDocument(qint64 key) = 0;
	virtual QByteArray GetDocument(const QByteArray& key) = 0;
	virtual bool HasRecord(qint64 key) = 0;
	virtual bool HasRecord(const QByteArray& key) = 0;
	virtual bool UpdateDocument(qint64 key, const QByteArray& data) = 0;
	virtual bool UpdateDocument(const QByteArray& key, const QByteArray& data) = 0;
	virtual qint64 GetKey(const QByteArray& value = QByteArray()) = 0;
	virtual qint64 GetFirstKey() = 0;
	virtual qint64 GetLastKey() = 0;
	virtual QString GetStringKey(const QByteArray& value = QByteArray()) = 0;
	virtual bool CreateIndex() = 0;
	virtual bool MoveTo(qint64 key) = 0;
	virtual bool MoveTo(const QByteArray& key) = 0;
	virtual bool MoveToFirst() = 0;
	virtual bool MoveToLast() = 0;
	virtual bool MoveToLowerBound(qint64 key) = 0;
	virtual bool MoveToLowerBound(const QByteArray& key) = 0;
	virtual bool MoveToNext() = 0;

};


}//namespace imtmdbx
