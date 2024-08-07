#pragma once

// ImtCore includes
#include <imtmdbx/IDocumentTable.h>
#include <imtmdbx/CDatabaseEngine.h>

// std includes
#include <iostream>


namespace imtmdbx
{


class CDocumentTable: virtual public IDocumentTable
{
public:
	CDocumentTable(const QString& name, mdbx::txn_managed& txn, mdbx::key_mode keyMode = mdbx::key_mode::reverse, mdbx::value_mode valueMode = mdbx::value_mode::single, bool hasIndex = false);

	//~CDocumentTable();

	// reimplemented (imtmdbx::IDocumentTable)
	virtual qint64 AddDocument(const QByteArray& data) override;
	virtual qint64 AddDocument(qint64 data) override;
	virtual qint64 AddDocument(const QByteArray& key, const QByteArray& data) override;
	virtual QByteArray GetDocument(qint64 key) override;
	virtual QByteArray GetDocument(const QByteArray& key) override;
	virtual bool HasRecord(qint64 key) override;
	virtual bool HasRecord(const QByteArray& key) override;
	virtual bool UpdateDocument(qint64 key,  const QByteArray& data) override;
	virtual bool UpdateDocument(const QByteArray& key, const QByteArray& data) override;
	virtual qint64 GetKey(const QByteArray& value) override;
	virtual QString GetStringKey(const QByteArray& value) override;
	virtual bool CreateIndex() override;
	bool CloseTable(mdbx::env_managed& env);

protected:
	virtual qint64 AddDocument(const char *data, int count, const QByteArray& keyStr = QByteArray());
	virtual bool UpdateDocument(const char *key, int count, const QByteArray& data);
	virtual bool Exists(const QString& name);

	QString m_tableName;
	mdbx::txn_managed& m_txn;
	mdbx::cursor_managed m_cursor;
	mdbx::map_handle m_mapHandle;
	mdbx::cursor_managed m_cursorIndex;
	mdbx::map_handle m_mapHandleIndex;
	bool m_hasIndex;
	mdbx::key_mode m_keyMode;
	mdbx::value_mode m_valueMode;
};

}//namespace imtmdbx
