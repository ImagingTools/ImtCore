// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtmdbx/IDocumentTable.h>

// 3rdParty includes
#include <mdbx.h++>


namespace imtmdbx
{


class CDocumentTable: virtual public IDocumentTable
{
public:
	CDocumentTable(
		const QString& name,
		mdbx::txn_managed& txn,
		mdbx::key_mode keyMode = mdbx::key_mode::reverse,
		mdbx::value_mode valueMode = mdbx::value_mode::single,
		bool hasIndex = false);
	~CDocumentTable();

	// reimplemented (imtmdbx::IDocumentTable)
	virtual quint64 AddDocument(const QByteArray& data) override;
	virtual quint64 AddDocument(quint64 data) override;
	virtual quint64 AddDocument(const QByteArray& key, const QByteArray& data) override;
	virtual QByteArray GetDocument() override;
	virtual QByteArray GetDocument(quint64 key) override;
	virtual QByteArray GetDocument(const QByteArray& key) override;
	virtual bool GetKey(quint64& key) const override;
	virtual bool GetKey(QByteArray& key) const override;
	virtual bool HasRecord(quint64 key) override;
	virtual bool HasRecord(const QByteArray& key) override;
	virtual bool UpdateDocument(quint64 key,  const QByteArray& data) override;
	virtual bool UpdateDocument(const QByteArray& key, const QByteArray& data) override;
	virtual bool GetKey(quint64& key, const QByteArray& value = QByteArray()) override;
	virtual bool GetFirstKey(quint64& key) override;
	virtual bool GetLastKey(quint64& key) override;
	virtual QByteArray GetKeyBA(const QByteArray& value = QByteArray()) override;
	virtual bool CreateIndex() override;
	virtual bool MoveTo(quint64 key) override;
	virtual bool MoveTo(const QByteArray& key) override;
	virtual bool MoveToFirst() override;
	virtual bool MoveToLast() override;
	virtual bool MoveToLowerBound(quint64 key) override;
	virtual bool MoveToLowerBound(const QByteArray& key) override;
	virtual bool MoveToUpperBound(quint64 key) override;
	virtual bool MoveToUpperBound(const QByteArray& key) override;
	virtual bool MoveToNext() override;
	virtual bool MoveToPrevious() override;
	virtual bool MoveToValue(const QByteArray& value) override;
	virtual bool RemoveDocument() override;
	virtual bool RemoveDocument(quint64 key, const QByteArray& value = QByteArray()) override;
	virtual bool RemoveDocument(const QByteArray& key, const QByteArray& value = QByteArray()) override;

	bool CloseTable(mdbx::env_managed& env);

protected:
	virtual quint64 AddDocument(
				const char *data,
				int count,
				const QByteArray& keyStr = QByteArray());
	virtual bool UpdateDocument(
				const char *key,
				int count,
				const QByteArray& data);
	virtual bool Exists(const QString& name);

protected:
	QString m_tableName;
	mdbx::txn_managed& m_txn;
	mdbx::cursor_managed m_cursor;
	mdbx::map_handle m_mapHandle;
	mdbx::cursor_managed m_cursorIndex;
	mdbx::map_handle m_mapHandleIndex;
	mdbx::key_mode m_keyMode;
	mdbx::value_mode m_valueMode;
	bool m_hasIndex;
};


} // namespace imtmdbx

