#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QByteArray>

namespace imtmdbx
{

class IDocumentTable
{
public:
	virtual qint64 AddDocument(const QByteArray& data) = 0;
	virtual qint64 AddDocument(qint64 data) = 0;
	virtual qint64 AddDocument(const QByteArray& key, const QByteArray& data) = 0;
	virtual QByteArray GetDocument(qint64 key) = 0;
	virtual bool UpdateDocument(qint64 key, const QByteArray& data) = 0;
	virtual qint64 GetKey(const QByteArray& value) = 0;
	virtual bool CreateIndex() = 0;
	virtual bool Exists(const QString& name) = 0;
};


}//namespace imtmdbx
